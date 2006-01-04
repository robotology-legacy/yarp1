
package yarp.os;

import java.io.*;
import java.net.*;

/*
  Don't really need to be this fancy, could have single threaded server
 */

class TelnetPort extends BasicPort {
    CommandProcessor processor;

    public TelnetPort(Address address, CommandProcessor processor) {
	super(address,"telnet");
	this.processor = processor;
    }


    private class TelnetPortlet extends Portlet {
	BasicPort owner;
	Socket socket;

	TelnetPortlet(BasicPort owner, Socket socket) {
	    this.owner = owner;
	    this.socket = socket;
	}

	public void close() {
	}

	public String apply(String cmd) {
	    return processor.apply(cmd);
	}

	public void run() {
	    System.out.println("telnet running");
	    try {
		StringBuffer buf = new StringBuffer("");
		boolean done = false;
		while (!done) {
		    int v = socket.getInputStream().read();
		    char ch = (char)v;
		    //System.out.println("Got " + v);
		    if (ch>=32) {
			buf.append(ch);
		    }
		    if (ch=='\n') {
			//System.out.println(buf);
			String response = apply(buf.toString());
			socket.getOutputStream().write(NetType.netString(response));
			socket.getOutputStream().flush();
			buf = new StringBuffer("");
			done = true;
		    }
		    if (v<0) { 
			throw new IOException();
		    }
		}
	    } catch (IOException e) {
		System.err.println("some problem with telnet");
	    }
	    System.out.println("telnet stopping");
	    try {
		socket.close();
	    } catch (IOException e) {
		System.err.println("some problem closing");
	    }
	    owner.removePortlet(this);
	}
    }

    public Portlet newPortlet(Socket socket) {
	return new TelnetPortlet(this,
				 socket);
    }

}

