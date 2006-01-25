package yarp.os;

import junit.framework.TestCase;

import java.util.*;

public class TextCarrierTest extends TestCase {
    Bottle bot;

    /**
     * Sets up the test fixture.
     *
     * Called before every test case method.
     */
    protected void setUp() {
        bot = new Bottle();
	List lst = new ArrayList();
	lst.add(new Integer(42));
	lst.add(new Integer(-17));
	lst.add("test");
	lst.add("\"test\"");
	lst.add("\"test\" \\backslash");
	for (Iterator it=lst.iterator(); it.hasNext(); ) {
	    Object o = it.next();
	    bot.add(o);
	}
    }

    /**
     * Tears down the test fixture.
     *
     * Called after every test case method.
     */
    protected void tearDown() {
        // release objects under test here, if necessary
    }

    public void testHeader() {
	TextCarrier tc = new TextCarrier();
	assertEquals(true,tc.checkHeader(new byte[] {'C','O','N','N','E','C','T',' '}));
    }
}
