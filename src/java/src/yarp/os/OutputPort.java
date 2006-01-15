
package yarp.os;

import java.io.*;

public class OutputPort implements Port {

    private ContentCreator creator;
    private Content content;
    private BasicPort port;

    public void creator(ContentCreator creator) {
	this.creator = creator;
    }

    public void register(String name) {
	/*
	Address server = NameClient.getNameClient().register(name);
	Logger.get().info("Registered output port " + name + " as " + 
			  server.toString());
	BasicPort basic = new BasicPort(server,name);
	*/
	BasicPort basic = new BasicPort(name);
	port = basic;
	basic.start();
    }

    public void connect(String src) {
	try {
	    port.connect(src);
	} catch (IOException e) {
	    Logger.get().error("Could not make connection");
	}
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
