package com.dewdrop623.androidcrypt;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class TestTest {
    @Test
    public void testAssertTrue() {
        assertTrue("string value 1".equals("string value 1"));
    }
    @Test
    public void testAssertFalse() {
        assertFalse("string value 1".equals("string value 2"));
    }
    @Test
    public void testFails() {
        assertEquals("string value 1","string value 2");
    }
}
