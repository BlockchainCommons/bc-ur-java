package com.bc.ur;

public class UREncoder extends NativeWrapper {

    public static String encode(UR ur) {
        return BCUR.UREncoder_encode(ur);
    }

    public UREncoder(UR ur, int maxFragmentLen, int firstSeqNum, int minFragmentLen) {
        super(BCUR.UREncoder_new(ur, maxFragmentLen, firstSeqNum, minFragmentLen));
    }

    public long getSeqNum() {
        return BCUR.UREncoder_seq_num(ptrObj);
    }

    public long getSeqLen() {
        return BCUR.UREncoder_seq_len(ptrObj);
    }

    public int[] getLastPartIndexes() {
        return BCUR.UREncoder_last_part_indexes(ptrObj);
    }

    public boolean isComplete() {
        return BCUR.UREncoder_is_complete(ptrObj);
    }

    public boolean isSinglePart() {
        return BCUR.UREncoder_is_single_part(ptrObj);
    }

    public String nextPart() {
        return BCUR.UREncoder_next_part(ptrObj);
    }
}
