package com.bc.ur

import androidx.test.ext.junit.runners.AndroidJUnit4
import com.bc.ur.URJni.UR_new_from_len_seed_string
import org.junit.Assert.*
import org.junit.Test
import org.junit.runner.RunWith


@RunWith(AndroidJUnit4::class)
class URDecoderTest {

    @Test
    fun testDecodeSinglePart() {
        val expectedUR = UR_new_from_len_seed_string(50, "Wolf")
        val encoded =
            "ur:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch"
        val ur = URDecoder.decode(encoded)
        assertEquals(expectedUR.type, ur.type)
        assertTrue(expectedUR.cbor.toTypedArray().contentDeepEquals(ur.cbor.toTypedArray()))
    }

    @Test
    fun testDecodeMultiParts() {
        val ur = UR_new_from_len_seed_string(32767, "Wolf")

        val encoder = UREncoder(ur, 1000, 100, 10)
        val decoder = URDecoder()

        encoder.use { e ->
            decoder.use { d ->

                var processPartCount = 0L
                do {
                    assertTrue(d.estimatedPercentComplete() < 1.0)

                    val part = e.nextPart()
                    d.receivePart(part)

                    assertEquals(33L, d.expectedPartCount())
                    assertEquals(++processPartCount, d.processedPartsCount())
                } while (!d.isComplete)

                assertTrue(d.isSuccess)
                assertFalse(d.isFailed)
                assertEquals(1.0, d.estimatedPercentComplete(), 1.0)


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