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

        assertThrows<URException>("UR.create(\"|\", bytes)") { UR.create("|", bytes) }
        assertThrows<URException>("UR.create(\"psbt@\", bytes)") { UR.create("psbt@", bytes) }
        assertThrows<URException>("UR.create(\"\", bytes)") { UR.create("", bytes) }
        assertThrows<URException>("UR.create(\"123|@345\", bytes)") { UR.create("123|@345", bytes) }
    }
}