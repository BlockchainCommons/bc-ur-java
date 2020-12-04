package com.bc.ur.util;

public class TestUtils {
    public static Byte[] toTypedArray(byte[] bytes) {
        Byte[] result = new Byte[bytes.length];
        for (int i = 0; i < bytes.length; i++)
            result[i] = bytes[i];
        return result;
    }

    public static String bytes2Hex(byte[] bytes) {
        StringBuilder builder = new StringBuilder();
        for (byte b : bytes) {
            builder.append(String.format("%x", b));
        }
        return builder.toString();
    }

    @SuppressWarnings("unchecked")
    public static <T extends Throwable> T assertThrows(String msg,
                                                       Class<T> expectedError,
                                                       Callable callable) {
        try {
            callable.call();
            throw new RuntimeException(msg);
        } catch (Throwable e) {
            if (expectedError.isAssignableFrom(e.getClass()))
                return (T) e;
            throw e;
        }
    }
}
