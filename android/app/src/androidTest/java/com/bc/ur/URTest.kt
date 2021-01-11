package com.bc.ur

import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Assert
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class URTest {

    @Test
    fun testCreateURFromMessage() {
        val bytes = byteArrayOf(0x01, 0x03, 0x7F, 0x3A, 0x11, 0x54, 0x12)
        val ur = UR.create("psbt", bytes)
        Assert.assertEquals("psbt", ur.type)
        Assert.assertEquals("137f3a115412", bytes2Hex(ur.message))
        Assert.assertEquals("47137f3a115412", bytes2Hex(ur.cbor))
    }
}