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

        encoder.use { e ->
            decoder.use { d ->
                do {
                    val part = e.nextPart()
                    d.receivePart(part)
                } while (!d.isComplete)

                // make sure resultUR return exact UR entered before
                val resultUR = d.resultUR()
                assertEquals(ur.type, resultUR.type)
                assertTrue(ur.cbor.toTypedArray().contentDeepEquals(resultUR.cbor.toTypedArray()))

                // make sure getting resultError throw URException
                assertThrows<URException>("test failed due to checking resultError") { d.resultError() }
            }
        }

        // make sure encoder/decoder is closed
        assertTrue(encoder.isClosed)
        assertTrue(decoder.isClosed)

        assertThrows<IllegalArgumentException>("test failed since encoder has not been disposed") { encoder.nextPart() }
        assertThrows<IllegalArgumentException>("test failed since decoder has not been disposed") { decoder.expectedType() }
    }

    @Test
    fun testDecodeError() {
        arrayOf(
            "",
            "ur:bytes/",
            "ur:ur:ur",
            "uf:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch"
        ).forEach {
            assertThrows<URException>("test failed due to $it") { URDecoder.decode(it) }
        }

    }
}