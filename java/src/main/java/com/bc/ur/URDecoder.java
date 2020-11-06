package com.bc.ur;

public class URDecoder extends NativeWrapper {

    public URDecoder() {
        super(BCUR.URDecoder_new());
    }

    public static UR decode(String encoded) {
        return BCUR.URDecoder_decode(encoded);
    }

    public String expectedType() {
        return BCUR.URDecoder_expected_type(ptrObj);
    }

    public long expectedPartCount() {
        return BCUR.URDecoder_expected_part_count(ptrObj);
    }

    public int[] receivedPartIndexes() {
        return BCUR.URDecoder_received_part_indexes(ptrObj);
    }

    public int[] lastPartIndexes() {
        return BCUR.URDecoder_last_part_indexes(ptrObj);
    }

    public long processedPartsCount() {
        return BCUR.URDecoder_processed_parts_count(ptrObj);
    }

    public double estimatedPercentComplete() {
        return BCUR.URDecoder_estimated_percent_complete(ptrObj);
    }

    public boolean isSuccess() {
        return BCUR.URDecoder_is_success(ptrObj);
    }

    public boolean isFailed() {
        return BCUR.URDecoder_is_failed(ptrObj);
    }

    public boolean isComplete() {
        return BCUR.URDecoder_is_complete(ptrObj);
    }

    public UR resultUR() {
        return BCUR.URDecoder_result_ur(ptrObj);
    }

    public IllegalStateException resultError() {
        return BCUR.URDecoder_result_error(ptrObj);
    }

    public boolean receivePart(String s) {
        return BCUR.URDecoder_receive_part(ptrObj, s);
    }
}
