package com.bc.ur;

import java.util.regex.Pattern;

import static com.bc.ur.URJni.UR_get_message;
import static com.bc.ur.URJni.UR_new_from_message;

public class UR {

    private static final String UR_TYPE_PATTERN = "^[a-z0-9-]+$";

    public static UR create(String type, byte[] message) {
        return UR_new_from_message(type, message);
    }

    public static UR create(byte[] message) {
        return create("bytes", message);
    }

    private final String type;

    private final byte[] cbor;

    private UR(String type, byte[] cbor) {
        validateType(type);
        this.type = type;
        this.cbor = cbor;
    }

    private void validateType(String type) {
        if (Pattern.compile(UR_TYPE_PATTERN).matcher(type).matches())
            return;
        throw new URException("Invalid UR type. Valid pattern is " + UR_TYPE_PATTERN);
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
