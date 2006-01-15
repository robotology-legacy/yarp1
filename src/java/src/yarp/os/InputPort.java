
package yarp.os;

import java.io.*;


/**
 * A port for receiving information.  The port is assigned a name
 * using the register method.  Then it is associated (using the
 * creator method) with a ContentCreator object that will be delegated
 * the work of decoding andin constructing input objects as they
 * arrive.
 */
public class InputPort implements Port {

    /**
     * Assign a unique global name to this port.
     * @param name The name to assign.
     */
    public void register(String name) {
	/*
	Address server = NameClient.getNameClient().register(name);
	Logger.get().info("Registered input port " + name + " as " + 
			  server.toString());
	BasicPort basic = new BasicPort(server,name);
	*/
	BasicPort basic = new BasicPort(name);
	basic.setHandler(handler);
	if (creator==null) {
	    Logger.get().error("should call creator before register");
	    System.exit(1);
	}
	content = creator.create();
	port = basic;
	basic.start();
    }


    /**
     * Wait for input to arrive.  Input can be extracted using the
     * content method.  Must have called register and creator
     * once.
     * @return true is input is available.
     */
    public boolean read() {
	try {
	    synchronized(stateMutex) {
		clientReading = false;
	    }
	    synchronized(readSomething) {
		readSomething.wait();
	    }
	    return true;
	} catch (InterruptedException e) {
	    return false;
	}
    }


    /**
     * Shut the port down.
     */
    public void close() {
	port.close();
    }


    /**
     * Access input that has arrived.  Must call read method first.
     * @return the input that has arrived.
     */
    public Object content() {
	synchronized(stateMutex) {
	    clientReading = true;
	}
	return content.object();
    }


    /**
     * Assign the object delegated to create input objects.
     * @param creator the object delegated to create input objects.
     */
    public void creator(ContentCreator creator) {
	this.creator = creator;
    }


    private class InputPortHandler implements ProtocolHandler  {

	public void read(Protocol proto) {
	    Logger.get().println("Could read now!");
	    if (content!=null) {
		try {
		    content.read(proto);
		} catch (IOException e) {
		    Logger.get().error("Problem reading");
		}
	    }
	    synchronized(readSomething) {
		readSomething.notify();
	    }
	}
	
	public void write(Protocol proto) {
	    // cannot do this from input port
	}
    }

    private ContentCreator creator;
    private BasicPort port;
    private Object readSomething = new Object();
    private Object stateMutex = new Object();
    private boolean clientReading = false;
    private Content content;
    private ProtocolHandler handler = new InputPortHandler();
}

