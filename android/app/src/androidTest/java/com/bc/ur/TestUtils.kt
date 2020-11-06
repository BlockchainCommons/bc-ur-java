package com.bc.ur

fun bytes2Hex(bytes: ByteArray): String? {
    val builder = StringBuilder()
    for (b in bytes) {
        builder.append(String.format("%x", b))
    }
    return builder.toString()
}