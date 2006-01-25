package yarp.os;

import junit.framework.TestCase;

import java.util.*;

public class BottleTest extends TestCase {

    private Bottle bot;
    private List lst;

    /**
     * Sets up the test fixture.
     *
     * Called before every test case method.
     */
    protected void setUp() {
        bot = new Bottle();
	lst = new ArrayList();
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

    public void testAdd() {
	bot.clear();
	assertEquals(0, bot.size());
	bot.add(new Integer(1));
	assertEquals(1, bot.size());
	bot.add("hello");
	assertEquals(2, bot.size());
    }

    public void testParse() {
	Bottle bot2 = new Bottle();
	bot2.fromString(bot.toString());
	assertEquals(lst,bot2.asList());
    }

    public void testBinary() {
	Bottle bot2 = new Bottle();
	bot2.set(bot.get());
	assertEquals(bot.asList(),bot2.asList());
    }
}
