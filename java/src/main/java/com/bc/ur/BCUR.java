package com.bc.ur;

class BCUR {

    static {
        LibraryLoader.load("bc-ur");
    }

    // UR
    static native UR UR_new_from_len_seed_string(int len, String seed);

    static native UR UR_new_from_message(String type, byte[] message);

    static native byte[] UR_get_message(UR ur);

    // UREncoder
    static native String UREncoder_encode(UR ur);

    static native NativeWrapper.JniObject UREncoder_new(UR ur, int maxFragmentLen, int firstSeqNum, int minFragmentLen);

    static native long UREncoder_seq_num(NativeWrapper.JniObject encoder);

    static native long UREncoder_seq_len(NativeWrapper.JniObject encoder);

    static native int[] UREncoder_last_part_indexes(NativeWrapper.JniObject encoder);

    static native boolean UREncoder_is_complete(NativeWrapper.JniObject encoder);

    static native boolean UREncoder_is_single_part(NativeWrapper.JniObject encoder);

    static native String UREncoder_next_part(NativeWrapper.JniObject encoder);

    // URDecoder
    static native UR URDecoder_decode(String encoded);

    static native NativeWrapper.JniObject URDecoder_new();

    static native String URDecoder_expected_type(NativeWrapper.JniObject decoder);

    static native long URDecoder_expected_part_count(NativeWrapper.JniObject decoder);

    static native int[] URDecoder_received_part_indexes(NativeWrapper.JniObject decoder);

    static native int[] URDecoder_last_part_indexes(NativeWrapper.JniObject decoder);

    static native long URDecoder_processed_parts_count(NativeWrapper.JniObject decoder);

    static native double URDecoder_estimated_percent_complete(NativeWrapper.JniObject decoder);

    static native boolean URDecoder_is_success(NativeWrapper.JniObject decoder);

    static native boolean URDecoder_is_failed(NativeWrapper.JniObject decoder);

    static native boolean URDecoder_is_complete(NativeWrapper.JniObject decoder);

    static native UR URDecoder_result_ur(NativeWrapper.JniObject decoder);

    static native IllegalStateException URDecoder_result_error(NativeWrapper.JniObject decoder);

    static native boolean URDecoder_receive_part(NativeWrapper.JniObject decoder, String s);

}
