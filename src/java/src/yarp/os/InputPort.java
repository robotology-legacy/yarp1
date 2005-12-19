
package yarp.os;

import java.lang.*;

public class InputPort implements ProtocolHandler {
    private ContentCreator creator;
    private Port port;
    private Object readSomething = new Object();
    private Object stateMutex = new Object();
    private boolean clientReading = false;

    private Content content;

    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	port = new Port(server);
	port.setHandler(this);
	//creator = new BottleContent();
	content = creator.create();
	port.start();
    }

    public void read(Protocol proto) {
	System.out.println("Could read now!");
	if (content!=null) {
	    content.read(proto);
	}
	synchronized(readSomething) {
	    readSomething.notify();
	}
    }

    public void write(Protocol proto) {
    }

    public void create(ContentCreator creator) {
	this.creator = creator;
    }

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

    public Object content() {
	synchronized(stateMutex) {
	    clientReading = true;
	}
	return content.content();
    }

    public void close() {
	port.close();
    }
}

