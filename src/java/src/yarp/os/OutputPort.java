
package yarp.os;

import java.lang.*;
import java.io.*;

public class OutputPort {

    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	port = new Port(server,name);
	port.start();
    }

    public void connect(String name) {
	NameClient nc = NameClient.getNameClient();
	
	Address add = nc.query(name);
	
	if (add==null) {
	    System.err.println("Cannot find port " + name);
	    return;
	}
	
	Connection c = new Connection(add,port.getPortName(),name);
	port.addConnection(c);
    }

    public void send(Content content) {
	port.send(content);
    }

    public void close() {
	port.close();
    }

    private Port port;
}
