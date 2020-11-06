/*
 * SPDX-License-Identifier: ISC
 * Copyright © 2014-2019 Bitmark. All rights reserved.
 * Use of this source code is governed by an ISC
 * license that can be found in the LICENSE file.
 */
package com.bc.ur;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

class LibraryLoader {

    private static final String SODIUM_LIBRARY = "sodiumjni";

    enum CPU {
        /**
         * 32 bit legacy Intel
         */
        X86,

        /**
         * 64 bit AMD (aka EM64T/X64)
         */
        X86_64,

        /**
         * 32 bit ARM
         */
        ARM,

        /**
         * 64 bit ARM
         */
        AARCH64,

        /**
         * Unknown CPU architecture.  A best effort will be made to infer architecture
         * specific values such as address and long size.
         */
        UNKNOWN;

        /**
         * Returns a {@code String} object representing this {@code CPU} object.
         *
         * @return the name of the cpu architecture as a lower case {@code String}.
         */
        @Override
        public String toString() {
            return name().toLowerCase();
        }

        static CPU getCPU() {
            String name = System.getProperty("os.arch");
            if (name == null) {
                throw new IllegalStateException("Unsupported CPU");
            }
            if (name.equalsIgnoreCase("x86") || name.equalsIgnoreCase("i386") || name
                    .equalsIgnoreCase("i486") || name.equalsIgnoreCase("i586") || name
                    .equalsIgnoreCase("i686") || name.equalsIgnoreCase("i786") || name
                    .equalsIgnoreCase("i86pc")) {
                return X86;
            } else if (name.equalsIgnoreCase("x86_64") || name.equalsIgnoreCase(
                    "amd64")) {
                return X86_64;
            } else if (name.equalsIgnoreCase("aarch64") || name.contains("armv8")) {
                return AARCH64;
            } else if (name.contains("arm")) {
                return ARM;
            } else {
                return UNKNOWN;
            }
        }

        String getAbi() {
            switch (this) {
                case ARM:
                    return "armeabi-v7a";
                case AARCH64:
                    return "arm64-v8a";
                case X86:
                    return "x86";
                case X86_64:
                    return "x86_64";
                default:
                    return null;
            }
        }
    }

    enum OS {
        /**
         * MacOSX
         */
        DARWIN,
        /**
         * Linux
         */
        LINUX,
        /**
         * Android
         */
        ANDROID,
        /**
         * The evil borg operating system
         */
        WINDOWS,
        /**
         * No idea what the operating system is
         */
        UNKNOWN;

        @Override
        public String toString() {
            return name().toLowerCase();
        }

        static OS getOS() {
            String osName = System.getProperty("os.name").split(" ")[0];

            if (osName.equalsIgnoreCase("mac") || osName.equalsIgnoreCase(
                    "darwin")) {
                return OS.DARWIN;
            } else if (osName.equalsIgnoreCase("linux")) {
                String javaVM = System.getProperty("java.vm.name");
                String javaVendor = System.getProperty("java.vendor");

                if (javaVendor.contains("Android") || javaVM.equalsIgnoreCase(
                        "dalvik")
                        || javaVM.equalsIgnoreCase("art")) {
                    return OS.ANDROID;
                } else {
                    return OS.LINUX;
                }
            } else if (osName.equalsIgnoreCase("windows")) {
                return OS.WINDOWS;
            } else {
                return OS.UNKNOWN;
            }
        }
    }

    static void load(String name) {
        try {
            loadNativeLib(name);
        } catch (IOException e) {
            throw new IllegalStateException("Unsupported platform " + e.getMessage());
        }
    }

    private static void loadNativeLib(String name) throws IOException {
        try {
            // load from default library search path
            System.loadLibrary(name);
        } catch (UnsatisfiedLinkError ignore) {
            // try to load from attach lib as resource
            InputStream inputStream = getLibStream(name);
            FileOutputStream outputStream = null;
            File tempFile = null;
            try {
                tempFile = File.createTempFile(
                        "native-lib",
                        "." + getLibExtension()
                );
                tempFile.deleteOnExit();
                outputStream = new FileOutputStream(tempFile);
                byte[] buffer = new byte[1024 * 8];
                int byteRead;
                while ((byteRead = inputStream.read(buffer)) != -1) {
                    outputStream.write(buffer, 0, byteRead);
                }

                outputStream.close();
                outputStream = null;

                try {
                    System.load(tempFile.getAbsolutePath());
                    tempFile.delete();
                } catch (Throwable e) {
                    System.loadLibrary(SODIUM_LIBRARY);
                }
            } catch (IOException e) {
                throw new IOException("Failed to create temporary file with " + e.getMessage());
            } finally {
                try {
                    if (outputStream != null) {
                        outputStream.close();
                    }
                    inputStream.close();
                    if (tempFile != null && tempFile.exists()) {
                        tempFile.delete();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private static InputStream getLibStream(String name) {

        // stream with expected ABI
        String libName = name + "." + getLibExtension();
        String path = getLibPath() + "/" + libName;
        InputStream stream = getResourceAsStream(path);
        if (stream != null) {
            return stream;
        }

        if (OS.getOS() == OS.ANDROID) {
            // stream with unexpected ABI
            String[] abis = {
                    CPU.ARM.getAbi(),
                    CPU.AARCH64.getAbi(),
                    CPU.X86.getAbi(),
                    CPU.X86_64.getAbi()
            };
            for (String abi : abis) {
                path = "lib/" + abi + "/" + libName;
                stream = getResourceAsStream(path);
                if (stream != null) {
                    return stream;
                }
            }
        }

        throw new IllegalStateException(
                "Couldn't load native library with path " + path);
    }

    private static InputStream getResourceAsStream(String name) {
        ClassLoader[] loaders = new ClassLoader[]{
                ClassLoader.getSystemClassLoader(),
                LibraryLoader.class.getClassLoader(),
                Thread.currentThread().getContextClassLoader()
        };
        for (ClassLoader loader : loaders) {
            InputStream stream = loader.getResourceAsStream(name);
            if (stream != null) {
                return stream;
            }
        }
        return null;
    }

    private static String getLibPath() {
        CPU cpu = CPU.getCPU();
        OS os = OS.getOS();
        if (cpu == CPU.UNKNOWN || os == OS.UNKNOWN) {
            throw new IllegalStateException("Unsupported platform");
        }
        if (os == OS.DARWIN || os == OS.LINUX) {
            return "lib";
        } else if (os == OS.ANDROID) {

            String abi = cpu.getAbi();
            if (abi == null) {
                throw new IllegalStateException(
                        "Unknown or does not support CPU architecture");
            }
            return "lib/" + abi;
        } else {
            throw new IllegalStateException("Unknown or does not support OS");
        }
    }

    private static String getLibExtension() {
        switch (OS.getOS()) {
            case WINDOWS:
                return "dll";
            case ANDROID:
                return "so";
            case LINUX:
                return "so";
            case DARWIN:
                return "dylib";
            default:
                throw new IllegalStateException(
                        "Unknown or does not support OS");
        }
    }

}
