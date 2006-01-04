
package yarp.os;

public class Time {
    public static void delay(double seconds) {
	try {
	    Thread.sleep((int)(seconds*1000));
	} catch (Exception e) {
	    System.out.println("delay failed");
	}
    }
}

