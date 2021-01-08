package com.bc.ur;

import static com.bc.ur.URJni.URDecoder_decode;
import static com.bc.ur.URJni.URDecoder_dispose;
import static com.bc.ur.URJni.URDecoder_estimated_percent_complete;
import static com.bc.ur.URJni.URDecoder_expected_part_count;
import static com.bc.ur.URJni.URDecoder_expected_type;
import static com.bc.ur.URJni.URDecoder_is_complete;
import static com.bc.ur.URJni.URDecoder_is_failed;
import static com.bc.ur.URJni.URDecoder_is_success;
import static com.bc.ur.URJni.URDecoder_last_part_indexes;
import static com.bc.ur.URJni.URDecoder_new;
import static com.bc.ur.URJni.URDecoder_processed_parts_count;
import static com.bc.ur.URJni.URDecoder_receive_part;
import static com.bc.ur.URJni.URDecoder_received_part_indexes;
import static com.bc.ur.URJni.URDecoder_result_error;
import static com.bc.ur.URJni.URDecoder_result_ur;

public class URDecoder extends NativeWrapper {

    public URDecoder() {
        super(URDecoder_new());
    }

    public static UR decode(String encoded) {
        return URDecoder_decode(encoded);
    }

    public String expectedType() {
        return URDecoder_expected_type(ptrObj);
    }

    public long expectedPartCount() {
        return URDecoder_expected_part_count(ptrObj);
    }

    public int[] receivedPartIndexes() {
        return URDecoder_received_part_indexes(ptrObj);
    }

    public int[] lastPartIndexes() {
        return URDecoder_last_part_indexes(ptrObj);
    }

    public long processedPartsCount() {
        return URDecoder_processed_parts_count(ptrObj);
    }

    public double estimatedPercentComplete() {
        return URDecoder_estimated_percent_complete(ptrObj);
    }

    public boolean isSuccess() {
        return URDecoder_is_success(ptrObj);
    }

    public boolean isFailed() {
        return URDecoder_is_failed(ptrObj);
    }

    public boolean isComplete() {
        return URDecoder_is_complete(ptrObj);
    }

    public UR resultUR() {
        return URDecoder_result_ur(ptrObj);
    }

    public URException resultError() {
        return URDecoder_result_error(ptrObj);
    }

    public boolean receivePart(String s) {
        return URDecoder_receive_part(ptrObj, s);
    }

    @Override
    public void close() throws Exception {
        if (isClosed() || !URDecoder_dispose(ptrObj))
            return;
        ptrObj = null;
    }
}
