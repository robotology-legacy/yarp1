
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
	Carrier carrier;

	TelnetPortlet(BasicPort owner, Carrier carrier) {
	    this.owner = owner;
	    this.carrier = carrier;
	}

	public void close() {
	}

	synchronized public String apply(String cmd, Address address) {
	    return processor.apply(cmd,address);
	}


	public void run() {
	    //System.out.println("telnet running");
	    /*
	    InetSocketAddress a2 = 
		(InetSocketAddress)socket.getRemoteSocketAddress();
	    InetAddress a3 = a2.getAddress();
	    Address remote = new Address(a3.getHostAddress(),
					 socket.getPort());
	    */
	    Address remote = null;
	    try {
		remote = carrier.getRemoteAddress();
		InetAddress ia = InetAddress.getByName(remote.getName());
		remote = new Address(ia.getHostAddress(),remote.getPort());
	    } catch (IOException e) {
		log.println("ok to skip: " + e);
	    }
	    log.println("remote address is " + remote);
	    try {
		//System.out.println(buf);
		boolean wait = false;
		String prefix = "";

		do {
		    String response = 
			apply(prefix + 
			      NetType.readLine(carrier.getInputStream()),
			      remote);
		    if (response==null) {
			wait = !wait;
			if (wait) {
			    response = "Welcome\n";
			    prefix = "NAME_SERVER ";
			}
		    }
		    if (response!=null) {
			carrier.getOutputStream().write(NetType.netString(response));
		    }
		    carrier.getOutputStream().flush();
		} while (wait);

	    } catch (IOException e) {
		System.err.println("some problem with telnet");
	    }
	    //System.out.println("telnet stopping");
	    try {
		carrier.close();
	    } catch (IOException e) {
		System.err.println("some problem closing");
	    }
	    owner.removePortlet(this);
	}
    }

    public Portlet newPortlet(Carrier carrier) {
	return new TelnetPortlet(this,
				 carrier);
    }

}

