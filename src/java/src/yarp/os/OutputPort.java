
package yarp.os;

import java.lang.*;
import java.io.*;

public class OutputPort {

    private ContentCreator creator;
    private Content content;

    public void creator(ContentCreator creator) {
	this.creator = creator;
    }

    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	port = new BasicPort(server,name);
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

    public Object content() {
	if (content==null) {
	    content = creator.create();
	}
	return content.object();
    }

    public void write() {
	port.send(content);
	content = null;
    }

    public void write(Content content) {
	port.send(content);
    }

    public void close() {
	port.close();
    }

    private BasicPort port;
}
