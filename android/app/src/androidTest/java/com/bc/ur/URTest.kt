package com.bc.ur

import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Assert
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class URTest {

    @Test
    fun testCreateURFromSeedString() {
        val ur = UR.create(50, "Wolf")
        Assert.assertEquals("bytes", ur.type)
        Assert.assertEquals(
            "916ec65cf77cadf55cd7f9cda1a13026ddd42e905b77adc36e4f2d3ccba44f7f4f2de44f42d84c374a0e149136f25b018",
            bytes2Hex(ur.message)
        )
        Assert.assertEquals(
            "5832916ec65cf77cadf55cd7f9cda1a13026ddd42e905b77adc36e4f2d3ccba44f7f4f2de44f42d84c374a0e149136f25b018",
            bytes2Hex(ur.cbor)
        )
    }

    @Test
    fun testCreateURFromMessage() {
        val bytes = byteArrayOf(0x01, 0x03, 0x7F, 0x3A, 0x11, 0x54, 0x12)
        val ur = UR.create("psbt", bytes)
        Assert.assertEquals("psbt", ur.type)
        Assert.assertEquals("137f3a115412", bytes2Hex(ur.message))
        Assert.assertEquals("47137f3a115412", bytes2Hex(ur.cbor))
    }
}