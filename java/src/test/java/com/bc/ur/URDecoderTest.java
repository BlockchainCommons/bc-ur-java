package com.bc.ur;

import com.bc.ur.util.TestUtils;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import java.util.Arrays;

import static com.bc.ur.URJni.UR_new_from_len_seed_string;
import static com.bc.ur.util.TestUtils.assertThrows;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;


@RunWith(JUnit4.class)
public class URDecoderTest {

    @Test
    public void testDecodeSinglePart() {
        UR expectedUR = UR_new_from_len_seed_string(50, "Wolf");
        String encoded = "ur:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch";
        UR ur = URDecoder.decode(encoded);
        assertEquals(expectedUR.getType(), ur.getType());
        assertTrue(Arrays.deepEquals(TestUtils.toTypedArray(expectedUR.getCbor()),
                                     TestUtils.toTypedArray(ur.getCbor())));
    }

    @Test
    public void testDecodeMultiParts() throws Exception {
        UR ur = UR_new_from_len_seed_string(32767, "Wolf");

        UREncoder refEncoder;
        URDecoder refDecoder;

        try (UREncoder encoder = new UREncoder(ur, 1000, 100, 10);
             URDecoder decoder = new URDecoder()) {
            refEncoder = encoder;
            refDecoder = decoder;

            long processPartCount = 0L;

            do {
                assertTrue(decoder.estimatedPercentComplete() < 1.0);

                String part = encoder.nextPart();
                decoder.receivePart(part);

                assertEquals(33L, decoder.expectedPartCount());
                assertEquals(++processPartCount, decoder.processedPartsCount());
            } while (!decoder.isComplete());

            assertTrue(decoder.isSuccess());
            assertFalse(decoder.isFailed());
            assertEquals(1.0, decoder.estimatedPercentComplete(), 1.0);

            // make sure resultUR return exact UR entered before
            UR resultUR = decoder.resultUR();
            assertEquals(ur.getType(), resultUR.getType());
            assertTrue(Arrays.deepEquals(TestUtils.toTypedArray(ur.getCbor()),
                                         TestUtils.toTypedArray(resultUR.getCbor())));

            // make sure getting resultError throw URException
            assertThrows("test failed due to checking resultError",
                         URException.class,
                         decoder::resultError);
        }

        // make sure encoder/decoder is closed
        assertTrue(refEncoder.isClosed());
        assertTrue(refDecoder.isClosed());

        assertThrows("test failed since encoder has not been disposed",
                     IllegalArgumentException.class,
                     refEncoder::nextPart);
        assertThrows("test failed since decoder has not been disposed",
                     IllegalArgumentException.class,
                     refDecoder::expectedType);
    }

    @Test
    public void testDecodeError() {
        String[] invalidData = new String[]{"",
                                            "ur:bytes/",
                                            "ur:ur:ur",
                                            "uf:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch"};
        for (String it : invalidData) {
            assertThrows("test failed due to " + it, URException.class, () -> URDecoder.decode(it));
        }
    }
}