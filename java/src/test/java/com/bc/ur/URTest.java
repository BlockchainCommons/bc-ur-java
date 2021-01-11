package com.bc.ur;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import static com.bc.ur.util.TestUtils.assertThrows;
import static com.bc.ur.util.TestUtils.bytes2Hex;
import static org.junit.Assert.assertEquals;

@RunWith(JUnit4.class)
public class URTest {

    @Test
    public void testCreateURFromMessage() {
        byte[] bytes = new byte[]{0x01, 0x03, 0x7F, 0x3A, 0x11, 0x54, 0x12};
        UR ur = UR.create("psbt", bytes);
        assertEquals("psbt", ur.getType());
        assertEquals("137f3a115412", bytes2Hex(ur.getMessage()));
        assertEquals("47137f3a115412", bytes2Hex(ur.getCbor()));

        assertThrows("UR.create(\"|\", bytes)", URException.class, () -> UR.create("|", bytes));
        assertThrows("UR.create(\"psbt@\", bytes)",
                     URException.class,
                     () -> UR.create("psbt@", bytes));
        assertThrows("UR.create(\"\", bytes)", URException.class, () -> UR.create("", bytes));
        assertThrows("UR.create(\"123|@345\", bytes)",
                     URException.class,
                     () -> UR.create("123|@345", bytes));
    }
}
