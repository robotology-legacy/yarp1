
package yarp.os;

/*
  This should be just a wrapper for a real logging system
 */

class Logger {
    private Logger parent;
    private String prefix;

    private final static int SEVERE = 10;
    private final static int INFO = 9;
    private final static int WARNING = 8;
    private final static int MAJOR_DEBUG = 7;
    private final static int DEBUG = 6;

    private int low = WARNING;

    public Logger(String prefix, Logger parent) {
	this.parent = parent;
	this.prefix = prefix;
    }

    private void show(int level, String msg) {
	if (parent == null) {
	    if (level>=low) {
		System.err.println(prefix + msg);
	    }
	} else {
	    parent.show(level,prefix+": "+msg);
	}
    }

    public void info(String msg) {
	show(INFO,msg);
    }

    public void severe(String msg) {
	show(SEVERE,msg);
    }

    public void error(String msg) {
	show(SEVERE,msg);
    }

    public void warning(String msg) {
	show(WARNING,msg);
    }

    public void majorDebug(String msg) {
	show(MAJOR_DEBUG,msg);
    }

    public void debug(String msg) {
	show(DEBUG,msg);
    }

    public void println(String msg) {
	debug(msg);
    }

    public void showAll() {
	low = DEBUG;
    }

    private static Logger log = new Logger("yarp: ",null);

    public static Logger get() {
	return log;
    }
}

