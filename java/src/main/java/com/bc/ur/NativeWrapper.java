package com.bc.ur;

abstract class NativeWrapper {

    protected final JniObject ptrObj;

    NativeWrapper(JniObject ptrObj) {
        this.ptrObj = ptrObj;
    }

    static class JniObject {

        private final transient long ptr;

        public JniObject(final long ptr) {
            this.ptr = ptr;
        }

        public long getPtr() {
            return ptr;
        }
    }
}
