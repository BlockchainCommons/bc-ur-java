#include <jni.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include <cxxabi.h>
#include <bc-ur.hpp>

using namespace ur;

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

class PrimitiveJni {
public:
    static std::string copy_std_string(JNIEnv *env, jstring js) {
        const char *utf = env->GetStringUTFChars(js, nullptr);
        if (utf == nullptr) {
            // exception thrown: OutOfMemoryError
            env->ExceptionCheck();
            return std::string();
        } else if (env->ExceptionCheck()) {
            // exception thrown
            env->ReleaseStringUTFChars(js, utf);
            return std::string();
        }

        std::string name(utf);
        env->ReleaseStringUTFChars(js, utf);
        return name;
    }

    static jstring to_jstring(JNIEnv *env, const std::string *string) {
        if (string == nullptr) {
            return nullptr;
        }

        return env->NewStringUTF(string->c_str());
    }

    static std::vector<uint8_t> to_uint8_t_vector(JNIEnv *env, jbyteArray array) {
        jsize len = env->GetArrayLength(array);
        auto c_array = to_uint8_t_array(env, array);
        auto vector = std::vector<uint8_t>(c_array.get(), c_array.get() + len);
        return vector;
    }

    static std::unique_ptr<uint8_t> to_uint8_t_array(JNIEnv *env, jbyteArray array) {
        jsize len = env->GetArrayLength(array);
        jbyte *c_array = env->GetByteArrayElements(array, JNI_FALSE);
        auto ret = std::unique_ptr<uint8_t>(new uint8_t[len]);
        memcpy(ret.get(), c_array, len);
        env->ReleaseByteArrayElements(array, c_array, JNI_ABORT);
        return ret;
    }

    static jbyteArray to_jbyteArray(JNIEnv *env, const std::vector<uint8_t> &vector) {
        auto *c_array(const_cast<uint8_t *>(vector.data()));
        return to_jbyteArray(env, c_array, vector.size());
    }

    static jbyteArray to_jbyteArray(JNIEnv *env, uint8_t *array, jsize len) {
        jbyteArray j_array = env->NewByteArray(len);
        env->SetByteArrayRegion(j_array, 0, len, reinterpret_cast<const jbyte *>(array));
        return j_array;
    }

    static jintArray to_jintArray(JNIEnv *env, const std::set<size_t> &s) {
        std::vector<size_t> vector(s.begin(), s.end());
        jintArray j_array = env->NewIntArray(s.size());
        env->SetIntArrayRegion(j_array, 0, vector.size(), reinterpret_cast<jint *>(vector.data()));
        return j_array;
    }
};

// java.lang.IllegalArgumentException
class IllegalArgumentExceptionJni : public JavaException<IllegalArgumentExceptionJni> {
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

// com.bc.ur.URException
class URExceptionJni : public JavaException<URExceptionJni> {
private:
    static jmethodID get_constructor_mid(JNIEnv *env) {
        jclass jclazz = JavaClass::get_jclass(env, get_class_name().c_str());
        assert(jclazz != nullptr);

        jmethodID mid = env->GetMethodID(jclazz, "<init>", "(Ljava/lang/String;)V");
        assert(mid != nullptr);

        return mid;
    }

public:
    static std::string get_class_name() {
        return "com/bc/ur/URException";
    }

    static bool throw_new(JNIEnv *env, const std::string &s) {
        return JavaException::throw_new(env, s);
    }

    static bool throw_new(JNIEnv *env, const std::exception &ex) {
        return JavaException::throw_new(env, ex);
    }

    static jobject new_object(JNIEnv *env, const std::string &msg) {
        jclass jclazz = get_jclass(env, get_class_name().c_str());
        assert(jclazz != nullptr);

        jmethodID mid = get_constructor_mid(env);
        assert(mid != nullptr);

        jstring jmsg = PrimitiveJni::to_jstring(env, &msg);
        return env->NewObject(jclazz, mid, jmsg);
    }
};

// com.bc.ur.NativeWrapper$JniObject
class ObjectJni : public JavaClass {
private:
    static jclass get_jclass(JNIEnv *env) {
        return JavaClass::get_jclass(env, "com/bc/ur/NativeWrapper$JniObject");
    }

    static jmethodID get_constructor_mid(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid = env->GetMethodID(jclazz, "<init>", "(J)V");
        assert(mid != nullptr);
        return mid;
    }

    static jmethodID get_ptr_mid(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid =
                env->GetMethodID(jclazz, "getPtr", "()J");
        assert(mid != nullptr);
        return mid;
    }

public:
    static jobject new_object(JNIEnv *env, void *ptr) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        jmethodID mid = get_constructor_mid(env);
        assert(mid != nullptr);

        return env->NewObject(jclazz, mid, (jlong) (uintptr_t) ptr);
    }

    static void *get_object(JNIEnv *env, jobject obj) {
        jmethodID mid = get_ptr_mid(env);
        assert(mid != nullptr);

        return (void *) (uintptr_t) (env->CallLongMethod(obj, mid));
    }
};

// com.bc.ur.UR
class URJni : public JavaClass {
private:
    static jclass get_jclass(JNIEnv *env) {
        return JavaClass::get_jclass(env, "com/bc/ur/UR");
    }

    static jmethodID get_cbor_mid(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid =
                env->GetMethodID(jclazz, "getCbor", "()[B");
        assert(mid != nullptr);
        return mid;
    }

    static jmethodID get_type_mid(JNIEnv *env) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        static jmethodID mid =
                env->GetMethodID(jclazz, "getType", "()Ljava/lang/String;");
        assert(mid != nullptr);
        return mid;
    }

public:
    static jobject to_j_UR(JNIEnv *env, const std::string &type, const ByteVector &cbor) {
        jclass jclazz = get_jclass(env);
        assert(jclazz != nullptr);

        jmethodID mid = env->GetMethodID(jclazz, "<init>", "(Ljava/lang/String;[B)V");
        assert(mid != nullptr);

        jstring j_type = PrimitiveJni::to_jstring(env, &type);
        jbyteArray j_cbor = PrimitiveJni::to_jbyteArray(env, cbor);
        return env->NewObject(jclazz, mid, j_type, j_cbor);
    }

    static std::unique_ptr<UR> to_c_UR(JNIEnv *env, jobject obj) {
        auto j_type = (jstring) env->CallObjectMethod(obj, get_type_mid(env));
        auto j_cbor = (jbyteArray) env->CallObjectMethod(obj, get_cbor_mid(env));

        std::vector<uint8_t> cbor = PrimitiveJni::to_uint8_t_vector(env, j_cbor);
        std::string type = PrimitiveJni::copy_std_string(env, j_type);
        return std::make_unique<UR>(type, (ByteVector const &) cbor);
    }

};

template<typename T, class CALLABLE>
static T call(JNIEnv *env, T errorValReturn, CALLABLE func) {
    try {
        return func();
    } catch (const std::exception &e) {
        URExceptionJni::throw_new(env, e);
        return errorValReturn;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobject JNICALL
Java_com_bc_ur_URJni_UR_1new_1from_1len_1seed_1string(JNIEnv *env,
                                                      jclass clazz,
                                                      jint len,
                                                      jstring seed) {
    if (seed == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Seed is null");
        return nullptr;
    }

    return call<jobject>(env, nullptr, [&]() {
        auto c_seed = PrimitiveJni::copy_std_string(env, seed);
        auto rng = Xoshiro256(c_seed);
        auto msg = rng.next_data(len);
        ByteVector cbor;
        CborLite::encodeBytes(cbor, msg);
        return URJni::to_j_UR(env, "bytes", cbor);
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_URJni_UR_1new_1from_1message(JNIEnv *env,
                                            jclass clazz,
                                            jstring type,
                                            jbyteArray message) {
    if (message == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java message is null");
        return nullptr;
    }

    std::string c_type = "bytes";
    if (type != nullptr) {
        c_type = PrimitiveJni::copy_std_string(env, type);
    }

    return call<jobject>(env, nullptr, [&]() {
        ByteVector cbor;
        auto msg = PrimitiveJni::to_uint8_t_vector(env, message);
        CborLite::encodeBytes(cbor, msg);
        msg.clear();
        return URJni::to_j_UR(env, c_type, cbor);
    });
}

JNIEXPORT jbyteArray JNICALL
Java_com_bc_ur_URJni_UR_1get_1message(JNIEnv *env, jclass clazz, jobject ur) {
    if (ur == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java UR is null");
        return nullptr;
    }

    return call<jbyteArray>(env, nullptr, [&]() {
        auto c_ur = URJni::to_c_UR(env, ur);
        auto cbor = c_ur->cbor();
        auto i = cbor.begin();
        auto end = cbor.end();
        ByteVector msg;
        CborLite::decodeBytes(i, end, msg);
        return PrimitiveJni::to_jbyteArray(env, msg);
    });
}

JNIEXPORT jstring JNICALL
Java_com_bc_ur_URJni_UREncoder_1encode(JNIEnv *env, jclass clazz, jobject ur) {
    if (ur == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java UR is null");
        return nullptr;
    }

    return call<jstring>(env, nullptr, [&]() {
        auto c_ur = URJni::to_c_UR(env, ur);
        auto result = UREncoder::encode(*c_ur);
        return env->NewStringUTF((&result)->c_str());
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_URJni_UREncoder_1new(JNIEnv *env,
                                    jclass clazz,
                                    jobject ur,
                                    jint max_fragment_len,
                                    jint first_seq_num,
                                    jint min_fragment_len) {
    if (ur == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java UR is null");
        return nullptr;
    }

    return call<jobject>(env, nullptr, [&]() {
        auto c_ur = URJni::to_c_UR(env, ur);
        auto c_encoder = new UREncoder(*c_ur,
                                       max_fragment_len,
                                       first_seq_num,
                                       min_fragment_len);
        return ObjectJni::new_object(env, c_encoder);
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_URJni_UREncoder_1seq_1num(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_ERR;
    }

    return call<jlong>(env, JNI_ERR, [&]() {
        auto c_encoder = static_cast<UREncoder *>(ObjectJni::get_object(env, encoder));
        return (jlong) (unsigned long long) (c_encoder->seq_num());
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_URJni_UREncoder_1seq_1len(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_ERR;
    }

    return call<jlong>(env, JNI_ERR, [&]() {
        auto c_encoder = static_cast<UREncoder *>(ObjectJni::get_object(env, encoder));
        return (jlong) (unsigned long long) (c_encoder->seq_len());
    });
}

JNIEXPORT jintArray JNICALL
Java_com_bc_ur_URJni_UREncoder_1last_1part_1indexes(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return nullptr;
    }

    return call<jintArray>(env, nullptr, [&]() {
        auto c_encoder = static_cast<UREncoder *>(ObjectJni::get_object(env, encoder));
        return PrimitiveJni::to_jintArray(env, c_encoder->last_part_indexes());
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_UREncoder_1is_1complete(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_FALSE;
    }

    return call<jboolean>(env, JNI_FALSE, [&]() {
        auto c_encoder = static_cast<UREncoder *>(ObjectJni::get_object(env, encoder));
        return (jboolean) (c_encoder->is_complete());
    });

}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_UREncoder_1is_1single_1part(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_FALSE;
    }

    return call<jboolean>(env, JNI_FALSE, [&]() {
        auto c_encoder = static_cast<UREncoder *>(ObjectJni::get_object(env, encoder));
        return (jboolean) (c_encoder->is_single_part());
    });

}

JNIEXPORT jstring JNICALL
Java_com_bc_ur_URJni_UREncoder_1next_1part(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return nullptr;
    }

    return call<jstring>(env, nullptr, [&]() {
        auto c_encoder = static_cast<UREncoder *>(ObjectJni::get_object(env, encoder));
        auto result = c_encoder->next_part();
        return PrimitiveJni::to_jstring(env, &result);
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_URJni_URDecoder_1decode(JNIEnv *env, jclass clazz, jstring encoded) {
    if (encoded == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return call<jobject>(env, nullptr, [&]() {
        auto c_encoded = PrimitiveJni::copy_std_string(env, encoded);
        auto c_ur = URDecoder::decode(c_encoded);
        return URJni::to_j_UR(env, c_ur.type(), c_ur.cbor());
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_URJni_URDecoder_1new(JNIEnv *env, jclass clazz) {
    return call<jobject>(env, nullptr, [&]() {
        auto c_decoder = new URDecoder();
        return ObjectJni::new_object(env, c_decoder);
    });
}

JNIEXPORT jstring JNICALL
Java_com_bc_ur_URJni_URDecoder_1expected_1type(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return call<jstring>(env, nullptr, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        auto result = (c_decoder->expected_type()).value();
        return PrimitiveJni::to_jstring(env, &result);
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_URJni_URDecoder_1expected_1part_1count(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_ERR;
    }

    return call<jlong>(env, JNI_ERR, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        try {
            return (jlong) c_decoder->expected_part_count();
        } catch (const std::bad_optional_access &e) {
            return (jlong) -1;
        }
    });
}

JNIEXPORT jintArray JNICALL
Java_com_bc_ur_URJni_URDecoder_1received_1part_1indexes(JNIEnv *env,
                                                        jclass clazz,
                                                        jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return call<jintArray>(env, nullptr, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        const auto &result = c_decoder->received_part_indexes();
        return PrimitiveJni::to_jintArray(env, result);
    });

}

JNIEXPORT jintArray JNICALL
Java_com_bc_ur_URJni_URDecoder_1last_1part_1indexes(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return call<jintArray>(env, nullptr, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        const auto &result = c_decoder->last_part_indexes();
        return PrimitiveJni::to_jintArray(env, result);
    });
}

JNIEXPORT jlong JNICALL
Java_com_bc_ur_URJni_URDecoder_1processed_1parts_1count(JNIEnv *env,
                                                        jclass clazz,
                                                        jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_ERR;
    }

    return call<jlong>(env, JNI_ERR, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        return (jlong) c_decoder->processed_parts_count();
    });
}

JNIEXPORT jdouble JNICALL
Java_com_bc_ur_URJni_URDecoder_1estimated_1percent_1complete(JNIEnv *env,
                                                             jclass clazz,
                                                             jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_ERR;
    }

    return call<jdouble>(env, JNI_ERR, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        return (jdouble) c_decoder->estimated_percent_complete();;
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_URDecoder_1is_1success(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return call<jboolean>(env, JNI_FALSE, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        return (jboolean) c_decoder->is_success();
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_URDecoder_1is_1failed(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return call<jboolean>(env, JNI_FALSE, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        return (jboolean) c_decoder->is_failure();
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_URDecoder_1is_1complete(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return call<jboolean>(env, JNI_FALSE, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        return (jboolean) c_decoder->is_complete();
    });
}

JNIEXPORT jobject JNICALL
Java_com_bc_ur_URJni_URDecoder_1result_1ur(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return call<jobject>(env, nullptr, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        const auto &c_ur = c_decoder->result_ur();
        return URJni::to_j_UR(env, c_ur.type(), c_ur.cbor());
    });
}

JNIEXPORT jthrowable JNICALL
Java_com_bc_ur_URJni_URDecoder_1result_1error(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return nullptr;
    }

    return call<jthrowable>(env, nullptr, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        const auto &ex = c_decoder->result_error();
        auto name = std::string(typeid(ex).name()) + ":" + ex.what();
        return (jthrowable) URExceptionJni::new_object(env, name);
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_URDecoder_1receive_1part(JNIEnv *env,
                                              jclass clazz,
                                              jobject decoder,
                                              jstring s) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return call<jboolean>(env, JNI_FALSE, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        auto cs = PrimitiveJni::copy_std_string(env, s);
        return (jboolean) c_decoder->receive_part(cs);
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_URDecoder_1dispose(JNIEnv *env, jclass clazz, jobject decoder) {
    if (decoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Decoder is null");
        return JNI_FALSE;
    }

    return call(env, JNI_FALSE, [&]() {
        auto c_decoder = static_cast<URDecoder *>(ObjectJni::get_object(env, decoder));
        delete c_decoder;
        return true;
    });
}

JNIEXPORT jboolean JNICALL
Java_com_bc_ur_URJni_UREncoder_1dispose(JNIEnv *env, jclass clazz, jobject encoder) {
    if (encoder == nullptr) {
        IllegalArgumentExceptionJni::throw_new(env, "Error: Java Encoder is null");
        return JNI_FALSE;
    }

    return call(env, JNI_FALSE, [&]() {
        auto c_encoder = static_cast<UREncoder *>(ObjectJni::get_object(env, encoder));
        delete c_encoder;
        return true;
    });

}
#ifdef __cplusplus
}
#endif
