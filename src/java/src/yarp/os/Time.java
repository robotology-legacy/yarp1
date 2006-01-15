
package yarp.os;

/**
 * A collection of simple but important time-related utilities.
 */
public class Time {

    /**
     * Pause for a number of seconds. Will respect fractions of seconds,
     * getting as close to the required pause as possible on the specific
     * platform.
     *
     * @param seconds The number of (possibly fractional) seconds to pause.
     */
    public static void delay(double seconds) {
	try {
	    Thread.sleep((int)(seconds*1000));
	} catch (Exception e) {
	    System.out.println("delay failed");
	}
    }
}

