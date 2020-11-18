package com.bc.ur;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import java.util.Arrays;

import static org.junit.Assert.*;


@RunWith(JUnit4.class)
public class URDecoderTest {

    @Test
    public void testDecodeSinglePart() {
        UR expectedUR = UR.create(50, "Wolf");
        String encoded =
                "ur:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch";
        UR ur = URDecoder.decode(encoded);
        assertEquals(expectedUR.getType(), ur.getType());
        assertTrue(Arrays.deepEquals(TestUtils.toTypedArray(expectedUR.getCbor()), TestUtils.toTypedArray(ur.getCbor())));
    }

    @Test
    public void testDecodeMultiParts() throws Throwable {
        UR ur = UR.create(32767, "Wolf");

        UREncoder refEncoder;
        URDecoder refDecoder;

        try (UREncoder encoder = new UREncoder(ur, 1000, 100, 10);
             URDecoder decoder = new URDecoder()) {
            refEncoder = encoder;
            refDecoder = decoder;
            do {
                String part = encoder.nextPart();
                decoder.receivePart(part);
            } while (!decoder.isComplete());

            // make sure resultUR return exact UR entered before
            UR resultUR = decoder.resultUR();
            assertEquals(ur.getType(), resultUR.getType());
            assertTrue(Arrays.deepEquals(TestUtils.toTypedArray(ur.getCbor()), TestUtils.toTypedArray(resultUR.getCbor())));

            // make sure getting resultError throw IllegalStateException
            try {
                decoder.resultError();
                throw new Throwable("test failed due to checking resultError");
            } catch (IllegalStateException e) {
                assertTrue(true);
            }
        } catch (Exception e) {
            throw new RuntimeException("Test failed due to " + e.getMessage());
        }

        // make sure encoder/decoder is closed
        assertTrue(refEncoder.isClosed());
        assertTrue(refDecoder.isClosed());

        try {
            refEncoder.nextPart();
            throw new RuntimeException("test failed since encoder has not been disposed");
        } catch (IllegalArgumentException ignore) {
        }

        try {
            refDecoder.expectedType();
            throw new RuntimeException("test failed since decoder has not been disposed");
        } catch (IllegalArgumentException ignore) {
        }
    }

    @Test
    public void testDecodeError() {
        String[] invalidData = new String[]{
                "",
                "ur:bytes/",
                "ur:ur:ur",
                "uf:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch"
        };
        for (String it : invalidData) {
            try {
                URDecoder.decode(it);
                throw new RuntimeException("test failed due to " + it);
            } catch (IllegalStateException ignore) {
            }
        }
    }
}