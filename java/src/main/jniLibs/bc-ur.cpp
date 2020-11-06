#include <jni.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <set>
#include <vector>
#include <cxxabi.h>
#include "../../../../deps/bc-ur/src/bc-ur.hpp"

using namespace ur;

// ============== JNI utilities ==============
// @reference: https://github.com/facebook/rocksdb/blob/master/java/rocksjni/portal.h
class JavaClass {
public:
    /**
     * Gets and initializes a Java Class
     *
     * @param env A pointer to the Java environment
     * @param jclazz_name The fully qualified JNI name of the Java Class
     *     e.g. "java/lang/String"
     *
     * @return The Java Class or nullptr if one of the
     *     ClassFormatError, ClassCircularityError, NoClassDefFoundError,
     *     OutOfMemoryError or ExceptionInInitializerError exceptions is thrown
     */
    static jclass get_jclass(JNIEnv *env, const char *jclazz_name) {
        jclass jclazz = env->FindClass(jclazz_name);
        return jclazz;
    }
};

// Java Exception template
template<class DERIVED>
class JavaException : public JavaClass {
public:
    /**
     * Create and throw a java exception with the provided message
     *
     * @param env A pointer to the Java environment
     * @param msg The message for the exception
     *
     * @return true if an exception was thrown, false otherwise
     */
    static bool throw_new(JNIEnv *env, const std::string &msg) {
        jclass jclazz = JavaClass::get_jclass(env, DERIVED::get_class_name().c_str());
        if (jclazz == nullptr) {
            // exception occurred accessing class
            std::cerr << DERIVED::get_class_name() + "::throw_new - Error: unexpected exception!"
                      << std::endl;
            return env->ExceptionCheck();
        }

        const jint rs = env->ThrowNew(jclazz, msg.c_str());
        if (rs != JNI_OK) {
            // exception could not be thrown
            std::cerr
                    << DERIVED::get_class_name() + "::throw_new - Fatal: could not throw exception!"
                    << std::endl;
            return env->ExceptionCheck();
        }

        return true;
    }

    /**
     * Create and throw a java exception with the provided exception
     *
     * @param env A pointer to the Java environment
     * @param ex The exception has been thrown
     *
     * @return true if an exception was thrown, false otherwise
     */
    static bool throw_new(JNIEnv *env, const std::exception &ex) {
        auto className = abi::__cxa_demangle(typeid(ex).name(), nullptr, nullptr, nullptr);
        auto msg = std::string(className) + "::" + ex.what();
        return throw_new(env, msg);
    }
};

// ============= Primitive types utilities ==============
class PrimitiveJni : public JavaClass {
public:
    static std::string copy_std_string(JNIEnv *env, jstring js, jboolean *has_exception = nullptr) {
        const char *utf = env->GetStringUTFChars(js, nullptr);
        if (utf == nullptr) {
            // exception thrown: OutOfMemoryError
            env->ExceptionCheck();
            if (has_exception != nullptr) *has_exception = JNI_TRUE;
            return std::string();
        } else if (env->ExceptionCheck()) {
            // exception thrown
            env->ReleaseStringUTFChars(js, utf);
            if (has_exception != nullptr) *has_exception = JNI_TRUE;
            return std::string();
        }

        std::string name(utf);
        env->ReleaseStringUTFChars(js, utf);
        if (has_exception != nullptr) *has_exception = JNI_FALSE;
        return name;
    }

    static jstring to_jstring(JNIEnv *env, const std::string *string,
                              const bool treat_empty_as_null = false) {
        if (string == nullptr) {
            return nullptr;
        }

        if (treat_empty_as_null && string->empty()) {
            return nullptr;
        }

        return env->NewStringUTF(string->c_str());
    }

    static std::vector<uint8_t> to_vector_uint8_t(JNIEnv *env, jbyteArray jarray) {
        std::vector<uint8_t> vector;
        jsize jarray_len = env->GetArrayLength(jarray);
        vector.reserve(jarray_len);
        uint8_t *arrayPtr = to_uint8_t_ptr(env, jarray);
        for (jsize i = 0; i < jarray_len; i++) {
            vector.emplace_back(arrayPtr[i]);
        }
        return vector;
    }

    static uint8_t *to_uint8_t_ptr(JNIEnv *env, jbyteArray array) {
        jsize len = env->GetArrayLength(array);
        auto *buf = new uint8_t[len];
        env->GetByteArrayRegion(array, 0, len, reinterpret_cast<jbyte *>(buf));
        return buf;
    }

    static jbyteArray to_jbyteArray(JNIEnv *env, const std::vector<uint8_t> &vector) {
        auto *buf = const_cast<uint8_t *>(vector.data());
        return to_jbyteArray(env, buf, vector.size());
    }

    static jbyteArray to_jbyteArray(JNIEnv *env, uint8_t *buf, jsize len) {
        jbyteArray array = env->NewByteArray(len);
        env->SetByteArrayRegion(array, 0, len, reinterpret_cast<const jbyte *>(buf));
        return array;
    }

    static jintArray to_jintArray(JNIEnv *env, const std::set<size_t> &s) {
        auto len = s.size();
        auto *buf = new size_t[len];
        for (int i = 0; i < len; i++) {
            auto it = s.begin();
            buf[i] = *it;
            std::advance(it, 1);
        }
        auto jarray = to_jintArray(env, buf, len);
        delete[] buf;
        return jarray;
    }

    static jintArray to_jintArray(JNIEnv *env, size_t *buf, jsize len) {
        jintArray array = env->NewIntArray(len);
        env->SetIntArrayRegion(array, 0, len, reinterpret_cast<const jint *>(buf));
        return array;
    }
};

// ============= Portal for specific objects ===================
// The portal class for java.lang.IllegalArgumentException
class IllegalArgumentExceptionJni :
        public JavaException<IllegalArgumentExceptionJni> {
public:
    static std::string get_class_name() {
        return "java/lang/IllegalArgumentException";
    }

    /**
     * Create and throw a Java IllegalArgumentException with the provided error message
     *
     * @param env A pointer to the Java environment
     * @param s The error message
     *
     * @return true if an exception was thrown, false otherwise
     */
    static bool throw_new(JNIEnv *env, const std::string &s) {
        return JavaException::throw_new(env, s);
    }
};

// The portal class for java.lang.IllegalStateException
class IllegalStateExceptionJni :
        public JavaException<IllegalStateExceptionJni> {
public:
    static std::string get_class_name() {
        return "java/lang/IllegalStateException";
    }

    /**
     * Create and throw a Java IllegalStateException with the provided error message
     *
     * @param env A pointer to the Java environment
     * @param s The error message
     *
     * @return true if an exception was thrown, false otherwise
     */
    static bool throw_new(JNIEnv *env, const std::string &s) {
        return JavaException::throw_new(env, s);
    }

    static bool throw_new(JNIEnv *env, const std::exception &ex) {
        return JavaException::throw_new(env, ex);
    }

    static jmethodID construct_method_id(JNIEnv *env) {
        jclass jclazz = JavaClass::get_jclass(env, get_class_name().c_str());
        assert(jclazz != nullptr);

        jmethodID mid = env->GetMethodID(jclazz, "<init>", "(Ljava/lang/String;)V");
        assert(mid != nullptr);

        return mid;
    }

    static jobject create(JNIEnv *env, const std::string &msg) {
        jclass jclazz = get_jclass(env, get_class_name().c_str());
        assert(jclazz != nullptr);

        jmethodID mid = construct_method_id(env);
        assert(mid != nullptr);

        jstring jmsg = PrimitiveJni::to_jstring(env, &msg);
        return env->NewObject(jclazz, mid, jmsg);
    }
};

// com.bc.ur.NativeWrapper$JniObject
class ObjectJni : public JavaClass {
public:
    static jclass get_jclass(JNIEnv *env) {
        return JavaClass::get_jclass(env, "com/bc/ur/NativeWrapper$JniObject");
    }

    static jmethodID construct_method_id(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid =
                env->GetMethodID(jclazz, "<init>", "(J)V");
        assert(mid != nullptr);
        return mid;
    }

    static jmethodID get_ptr_method_id(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid =
                env->GetMethodID(jclazz, "getPtr", "()J");
        assert(mid != nullptr);
        return mid;
    }

    static jobject create(JNIEnv *env, void *ptr) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        jmethodID mid = construct_method_id(env);
        assert(mid != nullptr);

        return env->NewObject(jclazz, mid, (jlong) (uintptr_t) ptr);
    }

    static void *get(JNIEnv *env, jobject obj) {
        jmethodID mid = get_ptr_method_id(env);
        assert(mid != nullptr);

        void *ret;
        ret = (void *) (uintptr_t) (env->CallLongMethod(obj, mid));
        return ret;
    }
};

// com.bc.ur.UR
class URJni : public JavaClass {
public:
    static jclass get_jclass(JNIEnv *env) {
        return JavaClass::get_jclass(env, "com/bc/ur/UR");
    }

    static jmethodID get_cbor_method_id(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid =
                env->GetMethodID(jclazz, "getCbor", "()[B");
        assert(mid != nullptr);
        return mid;
    }

    static jmethodID get_type_method_id(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid =
                env->GetMethodID(jclazz, "getType", "()Ljava/lang/String;");
        assert(mid != nullptr);
        return mid;
    }

    static jobject to_java_UR(JNIEnv *env, const std::string &type, const ByteVector &cbor) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        jmethodID mid = env->GetMethodID(jclazz, "<init>",
                                         "(Ljava/lang/String;[B)V");
        assert(mid != nullptr);

        jstring jtype = PrimitiveJni::to_jstring(env, &type);
        jbyteArray jcbor = PrimitiveJni::to_jbyteArray(env, cbor);
        jobject obj = env->NewObject(jclazz, mid, jtype, jcbor);
        return obj;
    }

    static UR *to_c_UR(JNIEnv *env, jobject obj) {
        auto jtype = (jstring) env->CallObjectMethod(obj, URJni::get_type_method_id(env));
        auto jcbor = (jbyteArray) env->CallObjectMethod(obj, URJni::get_cbor_method_id(env));

        std::vector<uint8_t> cbor = PrimitiveJni::to_vector_uint8_t(env, jcbor);
        std::string type = PrimitiveJni::copy_std_string(env, jtype, nullptr);
        auto *result = new UR(type, (ByteVector const &) cbor);
        return result;
    }

};

template<typename T, class CALLABLE>
static T safetyCall(JNIEnv *env, T errorValReturn, CALLABLE func) {
    try {
        return func();
    } catch (const std::exception &e) {
        IllegalStateExceptionJni::throw_new(env, e);
        return errorValReturn;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobject JNICALL
Java_com_bc_ur_BCUR_UR_1new_1from_1len_1seed_1string(JNIEnv *env, jclass clazz, jint len,
                                                     jstring seed) {
    if (seed == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Seed is null");
        return nullptr;
    }

    return safetyCall<jobject>(env, nullptr, [&]() {
        auto cseed = PrimitiveJni::copy_std_string(env, seed);
        auto rng = Xoshiro256(cseed);
        auto msg = rng.next_data(len);
        ByteVector cbor;
        CborLite::encodeBytes(cbor, msg);
        auto jur = URJni::to_java_UR(env, "bytes", cbor);
        return jur;
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_BCUR_UR_1new_1from_1message(JNIEnv *env, jclass clazz, jstring type,
                                           jbyteArray message) {
    if (message == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java message is null");
        return nullptr;
    }

    std::string ctype = "bytes";
    if (type != nullptr) {
        ctype = PrimitiveJni::copy_std_string(env, type);
    }

    return safetyCall<jobject>(env, nullptr, [&]() {
        ByteVector cbor;
        auto msg = PrimitiveJni::to_vector_uint8_t(env, message);
        CborLite::encodeBytes(cbor, msg);
        auto jur = URJni::to_java_UR(env, ctype, cbor);
        return jur;
    });
}

JNIEXPORT jbyteArray JNICALL
Java_com_bc_ur_BCUR_UR_1get_1message(JNIEnv *env, jclass clazz, jobject ur) {
    if (ur == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java UR is null");
        return nullptr;
    }

    return safetyCall<jbyteArray>(env, nullptr, [&]() {
        auto cur = URJni::to_c_UR(env, ur);
        auto cbor = cur->cbor();
        auto i = cbor.begin();
        auto end = cbor.end();
        ByteVector msg;
        CborLite::decodeBytes(i, end, msg);
        return PrimitiveJni::to_jbyteArray(env, msg);
    });
}

JNIEXPORT jstring JNICALL
Java_com_bc_ur_BCUR_UREncoder_1encode(JNIEnv *env, jclass clazz, jobject ur) {
    if (ur == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java UR is null");
        return nullptr;
    }

    return safetyCall<jstring>(env, nullptr, [&]() {
        auto cur = URJni::to_c_UR(env, ur);
        auto result = UREncoder::encode(*cur);
        auto jresult = env->NewStringUTF((&result)->c_str());
        return jresult;
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_BCUR_UREncoder_1new(JNIEnv *env, jclass clazz, jobject ur,
                                   jint max_fragment_len, jint first_seq_num,
                                   jint min_fragment_len) {
    if (ur == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java UR is null");
        return nullptr;
    }

    return safetyCall<jobject>(env, nullptr, [&]() {
        auto cur = URJni::to_c_UR(env, ur);
        auto *encoder = new UREncoder(*cur, max_fragment_len, first_seq_num, min_fragment_len);
        auto result = ObjectJni::create(env, encoder);
        return result;
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_BCUR_UREncoder_1seq_1num(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_ERR;
    }

    return safetyCall<jlong>(env, JNI_ERR, [&]() {
        auto *obj = (UREncoder *) ObjectJni::get(env, encoder);
        auto result = (jlong) (unsigned long long) (obj->seq_num());
        return result;
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_BCUR_UREncoder_1seq_1len(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_ERR;
    }

    return safetyCall<jlong>(env, JNI_ERR, [&]() {
        auto *obj = (UREncoder *) ObjectJni::get(env, encoder);
        auto result = (jlong) (unsigned long long) (obj->seq_len());
        return result;
    });
}

JNIEXPORT jintArray JNICALL
Java_com_bc_ur_BCUR_UREncoder_1last_1part_1indexes(JNIEnv *env, jclass clazz,
                                                   jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return nullptr;
    }

    return safetyCall<jintArray>(env, nullptr, [&]() {
        auto *obj = (UREncoder *) ObjectJni::get(env, encoder);
        auto result = PrimitiveJni::to_jintArray(env, obj->last_part_indexes());
        return result;
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_BCUR_UREncoder_1is_1complete(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_FALSE;
    }

    return safetyCall<jboolean>(env, JNI_FALSE, [&]() {
        auto *obj = (UREncoder *) ObjectJni::get(env, encoder);
        auto result = (jboolean) (obj->is_complete());
        return result;
    });

}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_BCUR_UREncoder_1is_1single_1part(JNIEnv *env, jclass clazz,
                                                jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_FALSE;
    }

    return safetyCall<jboolean>(env, JNI_FALSE, [&]() {
        auto *obj = (UREncoder *) ObjectJni::get(env, encoder);
        auto result = (jboolean) (obj->is_single_part());
        return result;
    });

}

JNIEXPORT jstring JNICALL
Java_com_bc_ur_BCUR_UREncoder_1next_1part(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return nullptr;
    }

    return safetyCall<jstring>(env, nullptr, [&]() {
        auto *obj = (UREncoder *) ObjectJni::get(env, encoder);
        auto result = obj->next_part();
        auto jresult = PrimitiveJni::to_jstring(env, &result);
        return jresult;
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_BCUR_URDecoder_1decode(JNIEnv *env, jclass clazz, jstring encoded) {
    if (encoded == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return safetyCall<jobject>(env, nullptr, [&]() {
        auto cencoded = PrimitiveJni::copy_std_string(env, encoded);
        auto cur = URDecoder::decode(cencoded);
        auto jur = URJni::to_java_UR(env, cur.type(), cur.cbor());
        return jur;
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_BCUR_URDecoder_1new(JNIEnv *env, jclass clazz) {
    return safetyCall<jobject>(env, nullptr, [&]() {
        auto *decoder = new URDecoder();
        auto result = ObjectJni::create(env, decoder);
        return result;
    });
}

JNIEXPORT jstring JNICALL
Java_com_bc_ur_BCUR_URDecoder_1expected_1type(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return safetyCall<jstring>(env, nullptr, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto result = (obj->expected_type()).value();
        auto jresult = PrimitiveJni::to_jstring(env, &result);
        return jresult;
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_BCUR_URDecoder_1expected_1part_1count(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_ERR;
    }

    return safetyCall<jlong>(env, JNI_ERR, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto result = (jlong) obj->expected_part_count();
        return result;
    });
}

JNIEXPORT jintArray JNICALL
Java_com_bc_ur_BCUR_URDecoder_1received_1part_1indexes(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return safetyCall<jintArray>(env, nullptr, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        const auto &result = obj->received_part_indexes();
        auto jresult = PrimitiveJni::to_jintArray(env, result);
        return jresult;
    });

}

JNIEXPORT jintArray JNICALL
Java_com_bc_ur_BCUR_URDecoder_1last_1part_1indexes(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return safetyCall<jintArray>(env, nullptr, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        const auto &result = obj->last_part_indexes();
        auto jresult = PrimitiveJni::to_jintArray(env, result);
        return jresult;
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_BCUR_URDecoder_1processed_1parts_1count(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_ERR;
    }

    return safetyCall<jlong>(env, JNI_ERR, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto result = (jlong) obj->processed_parts_count();
        return result;
    });
}

JNIEXPORT jdouble JNICALL
Java_com_bc_ur_BCUR_URDecoder_1estimated_1percent_1complete(JNIEnv *env, jclass clazz,
                                                            jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_ERR;
    }

    return safetyCall<jdouble>(env, JNI_ERR, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto result = obj->estimated_percent_complete();
        return result;
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_BCUR_URDecoder_1is_1success(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return safetyCall<jboolean>(env, JNI_FALSE, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto result = (jboolean) obj->is_success();
        return result;
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_BCUR_URDecoder_1is_1failed(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return safetyCall<jboolean>(env, JNI_FALSE, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto result = (jboolean) obj->is_failure();
        return result;
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_BCUR_URDecoder_1is_1complete(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return safetyCall<jboolean>(env, JNI_FALSE, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto result = (jboolean) obj->is_complete();
        return result;
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_BCUR_URDecoder_1result_1ur(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return safetyCall<jobject>(env, nullptr, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        const auto &cur = obj->result_ur();
        auto jresult = URJni::to_java_UR(env, cur.type(), cur.cbor());
        return jresult;
    });
}

JNIEXPORT jthrowable JNICALL
Java_com_bc_ur_BCUR_URDecoder_1result_1error(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return safetyCall<jthrowable>(env, nullptr, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        const auto &ex = obj->result_error();
        auto name = std::string(typeid(ex).name()) + ":" + ex.what();
        auto jresult = (jthrowable) IllegalStateExceptionJni::create(env, name);
        return jresult;
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_BCUR_URDecoder_1receive_1part(JNIEnv *env, jclass clazz, jobject decoder,
                                             jstring s) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return safetyCall<jboolean>(env, JNI_FALSE, [&]() {
        auto *obj = (URDecoder *) ObjectJni::get(env, decoder);
        auto cs = PrimitiveJni::copy_std_string(env, s);
        auto result = (jboolean) obj->receive_part(cs);
        return result;
    });
}
#ifdef __cplusplus
}
#endif