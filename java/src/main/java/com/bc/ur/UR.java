package com.bc.ur;

import static com.bc.ur.URJni.UR_get_message;
import static com.bc.ur.URJni.UR_new_from_len_seed_string;
import static com.bc.ur.URJni.UR_new_from_message;

public class UR {

    public static UR create(int len, String seed) {
        return UR_new_from_len_seed_string(len, seed);
    }

    public static UR create(String type, byte[] message) {
        return UR_new_from_message(type, message);
    }

    public static UR create(byte[] message) {
        return create("bytes", message);
    }

    private final String type;

    private final byte[] cbor;

    private UR(String type, byte[] cbor) {
        this.type = type;
        this.cbor = cbor;
    }

    public byte[] getCbor() {
        return cbor;
    }

    public String getType() {
        return type;
    }

    public byte[] getMessage() {
        return UR_get_message(this);
    }
}
