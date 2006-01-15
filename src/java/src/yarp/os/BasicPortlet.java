
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;

class BasicPortlet extends Portlet {
    private BasicPort owner;
    private ShiftStream shift;
    private String sourceName;
    private String ownerName;
    private String carrierName;
    private static Logger log = Logger.get();

    public static void show(String from, String msg) {
	boolean admin = true;
	if (from!=null) {
	    if (from.length()>0) {
		if (from.charAt(0) == '/') {
		    admin = false;
		}
	    }
	}
	if (admin) {
	    log.println(msg);
	} else {
	    log.info(msg);
	}
    }

    public BasicPortlet(BasicPort owner, ShiftStream shift) {
	this.owner = owner;
	this.shift = shift;
	ownerName = owner.getPortName();
    }

    public void close() {
	log.println("Trying to halt portlet");
	try {
	    shift.close();
	} catch (IOException e) {
	    log.println("Problem while halting portlet");
	}
	interrupt();
    }

    public String getFromName() {
	return sourceName;
    }

    public String getToName() {
	return ownerName;
    }

    public String getCarrierName() {
	return carrierName;
    }

    private String stringify(byte[] txt) {
	return NetType.netString(txt);
    }

    

    public void run() {
	boolean done = false;
	Protocol protocol = null;
	try {
	// hand over control of this socket
	protocol = new Protocol(shift);

	protocol.expectHeader();
	protocol.respondToHeader();
	sourceName = protocol.getSender();

	carrierName = protocol.getCarrierName();

	show(getFromName(), 
	     "Receiving input from " + getFromName() + " to " + 
	     getToName() + " using " + carrierName);

	PrintStream ps = null;
	if (protocol.isTextMode()) {
	    ps = new PrintStream(protocol.getOutputStream());
	}

	while (protocol.isOk()&&!done) {

	    protocol.expectIndex();
	    //System.out.println("got index, responding..");
	    if (!protocol.respondToIndex()) { 
		log.error("Protocol problem");
		protocol.close();
		//System.exit(1); 
		return;
	    }
	    //System.out.println("got index, responded.");
	    //System.out.println("size is " + protocol.getSize());
	    int cmd = 0;
	    byte[] arg = new byte[] {0};
	    if (protocol.isTextMode()) {
		String txt = protocol.expectLine();
		if (txt.length()>0) {
		    cmd = txt.charAt(0);
		    arg = NetType.netString(txt);
		}
	    }
	    if (protocol.getSize()>=8) {
		log.println("waiting for 8");
		byte[] b = protocol.expectBlock(8);
		log.println("got 8");
		cmd = (int)b[5];
		log.println("command is " + cmd);
		if (b[4]==(byte)'~') {
		    if (protocol.getSize()>0 && cmd==0) {
			arg = protocol.expectBlock(-1);
			if (cmd==0) {
			    cmd = ((int)arg[0]);
			    log.println("Subbing cmd " + cmd);
			}
		    }
		} else {
		    log.warning("Bad message");
		}
	    }
	    if (cmd!=0) {
		log.println("Got command [" + ((char)cmd) + "]");
		String src = stringify(arg);
		switch(cmd) {
		case '!':
		    src = src.substring(1,src.length());
		    log.info("Disconnecting output from " + 
			     src);
		    owner.removeConnection(src,ps);
		    break;
		case '/':
		    owner.connect(src,ps);
		    break;
		case 'd':
		    log.println(protocol.getSender() + " sent data "
				+ " to " + "me");
		    owner.handleData(protocol);
		    break;
		case '~':
		    src = src.substring(1,src.length());
		    log.println("Should shut down input from " + src);
		    owner.removePortlet(src, ps);
		    break;
		case '*':
		    log.println("Should list connections");
		    String desc = owner.describe(this);
		    if (protocol.isTextMode()) {
			protocol.write(NetType.netString(desc));
		    } else {
			System.out.println(desc);
		    }
		    break;
		case 'q':
		    // all listeners shut down
		    owner.println(ps,"Bye bye");
		    ps.flush();
		    done = true;
		    break;
		}
	    }
	    protocol.expectBlock(protocol.getSize());
	    //protocol.respondToBlock();
	    protocol.sendAck();

	    log.println("Finished");
	}
	protocol.close();

	//clientSocket.close();
	} catch (IOException e) {
	    log.error("Portlet IO shutdown - reason " + e);
	    if (protocol!=null) {
		protocol.close();
	    }
	    // should call owner to forget me
	}

	show(getFromName(),
	     "Stopping input from " + getFromName() + " to " + getToName());
	owner.removePortlet(this);
    }
}
