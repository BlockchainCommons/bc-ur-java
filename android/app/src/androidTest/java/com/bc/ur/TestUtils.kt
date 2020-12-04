package com.bc.ur

fun bytes2Hex(bytes: ByteArray): String? {
    val builder = StringBuilder()
    for (b in bytes) {
        builder.append(String.format("%x", b))
    }
    return builder.toString()
}

inline fun <T : AutoCloseable?, R> T.use(block: (T) -> R): R {
    var exception: Exception? = null
    try {
        return block(this)
    } catch (e: Exception) {
        exception = e
        throw e
    } finally {
        when {
            this == null -> {
            }
            exception == null -> close()
            else ->
                try {
                    close()
                } catch (closeException: Exception) {
                    // cause.addSuppressed(closeException) // ignored here
                }
        }
    }
}

inline fun <reified T : Throwable> assertThrows(
    msg: String?,
    callable: () -> Unit
): T {
    try {
        callable()
        throw RuntimeException(msg)
    } catch (e: Throwable) {
        if (e is T) return e
        throw e
    }
}