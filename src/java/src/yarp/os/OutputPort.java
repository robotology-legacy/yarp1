
package yarp.os;

import java.lang.*;
import java.io.*;

public class OutputPort {

    private ContentCreator creator;
    private Content content;
    private BasicPort port;

    public void creator(ContentCreator creator) {
	this.creator = creator;
    }

    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	port = new BasicPort(server,name);
	port.start();
    }

    public void connect(String name) {
	if (port==null) {
	    System.out.println("Please call register() before connect()");
	    System.exit(1);
	}
	if (creator==null) {
	    System.out.println("Please call creator() before connect()");
	    System.exit(1);
	}

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
	content();
	port.send(content);
	content = null;
    }

    public void write(Content content) {
	port.send(content);
    }

    public void close() {
	port.close();
    }

}
