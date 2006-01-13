
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

	public String apply(String cmd, Address address) {
	    return processor.apply(cmd,address);
	}


	public void run() {
	    //System.out.println("telnet running");
	    InetSocketAddress a2 = 
		(InetSocketAddress)socket.getRemoteSocketAddress();
	    InetAddress a3 = a2.getAddress();
	    Address remote = new Address(a3.getHostAddress(),
					 socket.getPort());
	    //System.out.println("remote address is " + remote);
	    try {
		//System.out.println(buf);
		boolean wait = false;
		String prefix = "";

		do {
		    String response = 
			apply(prefix + 
			      NetType.readLine(socket.getInputStream()),
			      remote);
		    if (response==null) {
			wait = !wait;
			if (wait) {
			    response = "Welcome\n";
			    prefix = "NAME_SERVER ";
			}
		    }
		    if (response!=null) {
			socket.getOutputStream().write(NetType.netString(response));
		    }
		    socket.getOutputStream().flush();
		} while (wait);

	    } catch (IOException e) {
		System.err.println("some problem with telnet");
	    }
	    //System.out.println("telnet stopping");
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

