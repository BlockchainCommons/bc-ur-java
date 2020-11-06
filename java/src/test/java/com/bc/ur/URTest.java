package com.bc.ur;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import static com.bc.ur.TestUtils.bytes2Hex;
import static org.junit.Assert.assertEquals;

@RunWith(JUnit4.class)
public class URTest {

    @Test
    public void testCreateURFromSeedString() {
        UR ur = UR.create(50, "Wolf");
        assertEquals("bytes", ur.getType());
        assertEquals("916ec65cf77cadf55cd7f9cda1a13026ddd42e905b77adc36e4f2d3ccba44f7f4f2de44f42d84c374a0e149136f25b018", bytes2Hex(ur.getMessage()));
        assertEquals("5832916ec65cf77cadf55cd7f9cda1a13026ddd42e905b77adc36e4f2d3ccba44f7f4f2de44f42d84c374a0e149136f25b018", bytes2Hex(ur.getCbor()));
    }

    @Test
    public void testCreateURFromMessage() {
        byte[] bytes = new byte[]{0x01, 0x03, 0x7F, 0x3A, 0x11, 0x54, 0x12};
        UR ur = UR.create("psbt", bytes);
        assertEquals("psbt", ur.getType());
        assertEquals("137f3a115412", bytes2Hex(ur.getMessage()));
        assertEquals("47137f3a115412", bytes2Hex(ur.getCbor()));
    }
}
