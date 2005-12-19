
package yarp.os;

import java.lang.*;
import java.io.*;

public class OutputPort {
    private Port port;
    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	port = new Port(server);
	//port.setHandler(this);
	port.start();
    }

    public void connect(String name) {
	try {
	    NameClient nc = NameClient.getNameClient();
	    
	    Address add = nc.query(name);
	    
	    if (add==null) {
		System.err.println("Cannot find port to write to");
	    }
	    
	    Connection c = new Connection(add);
	    port.addConnection(c);
	} catch (IOException e) {
	    System.err.println("connection addition failed");
	}
    }

    public void addConnection(Address address) {
	try {
	    port.addConnection(new Connection(address));
	} catch (IOException e) {
	    System.err.println("connection addition failed");
	}
    }

    public void send(Content content) {
	port.send(content);
    }

    public void close() {
	port.close();
    }
}
