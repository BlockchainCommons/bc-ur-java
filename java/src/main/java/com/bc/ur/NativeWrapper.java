package com.bc.ur;

abstract class NativeWrapper implements AutoCloseable {

    protected JniObject ptrObj;

    NativeWrapper(JniObject ptrObj) {
        this.ptrObj = ptrObj;
    }

    public boolean isClosed() {
        return ptrObj == null;
    }

    static class JniObject {

        private final transient long ptr;

        JniObject(final long ptr) {
            this.ptr = ptr;
        }

        long getPtr() {
            return ptr;
        }
    }
}
