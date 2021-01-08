package com.bc.ur;

import static com.bc.ur.URJni.UREncoder_dispose;
import static com.bc.ur.URJni.UREncoder_encode;
import static com.bc.ur.URJni.UREncoder_is_complete;
import static com.bc.ur.URJni.UREncoder_is_single_part;
import static com.bc.ur.URJni.UREncoder_last_part_indexes;
import static com.bc.ur.URJni.UREncoder_new;
import static com.bc.ur.URJni.UREncoder_next_part;
import static com.bc.ur.URJni.UREncoder_seq_len;
import static com.bc.ur.URJni.UREncoder_seq_num;

public class UREncoder extends NativeWrapper {

    public static String encode(UR ur) {
        return UREncoder_encode(ur);
    }

    public UREncoder(UR ur, int maxFragmentLen, int firstSeqNum, int minFragmentLen) {
        super(UREncoder_new(ur, maxFragmentLen, firstSeqNum, minFragmentLen));
    }

    public UREncoder(UR ur, int maxFragmentLen) {
        this(ur, maxFragmentLen, 0, 10);
    }

    public long getSeqNum() {
        return UREncoder_seq_num(ptrObj);
    }

    public long getSeqLen() {
        return UREncoder_seq_len(ptrObj);
    }

    public int[] getLastPartIndexes() {
        return UREncoder_last_part_indexes(ptrObj);
    }

    public boolean isComplete() {
        return UREncoder_is_complete(ptrObj);
    }

    public boolean isSinglePart() {
        return UREncoder_is_single_part(ptrObj);
    }

    public String nextPart() {
        return UREncoder_next_part(ptrObj);
    }


    @Override
    public void close() throws Exception {
        if (isClosed() || !UREncoder_dispose(ptrObj))
            return;
        ptrObj = null;
    }
}
