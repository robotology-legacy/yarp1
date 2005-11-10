
package yarp.os;

import java.lang.*;

public class InputPort implements ProtocolHandler {
    private ContentCreator creator;
    private Port port;
    private Object readSomething = new Object();
    private Object stateMutex = new Object();
    private boolean clientReading = false;

    public void register(String name) {
	NameClient nc = new NameClient(new Address("localhost",10000));
	Address server = nc.register(name);
	port = new Port(server);
	port.setHandler(this);
	port.start();
    }

    public void read(Protocol proto) {
	System.out.println("Could read now!");
	synchronized(readSomething) {
	    readSomething.notify();
	}
    }

    public void write(Protocol proto) {
    }

    public void create(ContentCreator creator) {
	this.creator = creator;
    }

    public boolean read() throws InterruptedException {
	synchronized(stateMutex) {
	    clientReading = false;
	}
	synchronized(readSomething) {
	    readSomething.wait();
	}
	return true;
    }

    public Object get() {
	synchronized(stateMutex) {
	    clientReading = true;
	}
	return null;
    }
}

