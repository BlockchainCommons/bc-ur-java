package com.bc.ur

import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith


@RunWith(AndroidJUnit4::class)
class URDecoderTest {

    @Test
    fun testDecodeSinglePart() {
        val expectedUR = UR.create(50, "Wolf")
        val encoded =
            "ur:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch"
        val ur = URDecoder.decode(encoded)
        assertEquals(expectedUR.type, ur.type)
        assertTrue(expectedUR.cbor.toTypedArray().contentDeepEquals(ur.cbor.toTypedArray()))
    }

    @Test
    fun testDecodeMultiParts() {
        val ur = UR.create(32767, "Wolf")
        val encoder = UREncoder(ur, 1000, 100, 10)
        val decoder = URDecoder()
        do {
            val part = encoder.nextPart()
            decoder.receivePart(part)
        } while (!decoder.isComplete)

        // make sure resultUR return exact UR entered before
        val resultUR = decoder.resultUR()
        assertEquals(ur.type, resultUR.type)
        assertTrue(
            ur.cbor.toTypedArray().contentDeepEquals(resultUR.cbor.toTypedArray())
        )

        // make sure getting resultError throw IllegalStateException
        try {
            decoder.resultError()
            throw RuntimeException("test failed due to checking resultError")
        } catch (e: IllegalStateException) {
            assertTrue(true)
        }
    }

    @Test
    fun testDecodeError() {
        arrayOf(
            "",
            "ur:bytes/",
            "ur:ur:ur",
            "uf:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch"
        ).forEach {
            try {
                URDecoder.decode(it)
                throw RuntimeException("test failed due to $it")
            } catch (ignore: IllegalStateException) {
            }

        }

    }
}