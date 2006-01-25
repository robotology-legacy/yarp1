
package yarp.os;

import java.io.*;

// no mention of sockets!  keep it this way...

class BasicPortlet extends Portlet {
    private BasicPort owner;
    private String sourceName;
    private String ownerName;
    private String carrierName;
    private Logger log = Logger.get();
    private InputProtocol proto = null;

    public BasicPortlet(BasicPort owner, InputProtocol proto) {
	this.owner = owner;
	this.proto = proto;
	ownerName = owner.getPortName();
	log = new Logger(ownerName,Logger.get());
    }

    public void close() {
	log.println("Trying to halt portlet");
	try {
	    if (proto!=null) {
		proto.close();
	    }
	} catch (IOException e) {
	    log.println("Problem while halting portlet");
	}
	log.println("Progress halting portlet");
	interrupt();
	log.println("Done halting portlet");
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


    public void run() {
	// this is a temporary cheat
	//Protocol protocol = (Protocol)proto;
	InputProtocol protocol = proto;

	boolean done = false;
	try {
	// hand over control of this socket

	//protocol.setRoute(protocol.getRoute().addToName(ownerName));
	
	//protocol.expectHeader();
	//protocol.respondToHeader();
	protocol.initiate(ownerName);

	Route route = protocol.getRoute();
	sourceName = route.getFromName();
	carrierName = route.getCarrierName();

	show(getFromName(), 
	     "Receiving input from " + getFromName() + " to " + 
	     getToName() + " using " + carrierName);


	PrintStream reply = 
	    new PrintStream(protocol.getReplyStream());
	//new PrintStream(protocol.getStreams().getOutputStream());
	PrintStream ps = null;

	while (/*protocol.isOk()&&*/!done) {
	    /*
	    log.println("waiting for index");
	    protocol.expectIndex();
	    log.println("got index, responding..");
	    if (!protocol.respondToIndex()) { 
		log.error("Protocol problem");
		protocol.close();
		//System.exit(1); 
		return;
	    }
	    BlockReader reader = protocol.getReader();
	    */
	    BlockReader reader = protocol.beginRead();
	    //System.out.println("got index, responded.");
	    //System.out.println("size is " + protocol.getSize());

	    // PENDING: use CommandContent to do this reading
	    if (reader.isTextMode()) {
		ps = reply;
	    }
	    int cmd = 0;
	    byte[] arg = new byte[] {0};
	    if (reader.isTextMode()) {
		String txt = reader.expectLine();
		if (txt.length()>0) {
		    cmd = txt.charAt(0);
		    arg = NetType.netString(txt);
		}
	    }
	    if (reader.getSize()>=8) {
		log.println("waiting for 8");
		byte[] b = reader.expectBlock(8);
		log.println("got 8");
		cmd = (int)b[5];
		log.println("command is " + cmd);
		if (b[4]==(byte)'~') {
		    if (reader.getSize()>0 && cmd==0) {
			arg = reader.expectBlock(-1);
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
		String src = NetType.netString(arg);
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
		    log.println(protocol.getRoute().getFromName() + 
				" sent data " + " to " + "me");
		    owner.handleData(reader);
		    break;
		case '~':
		    src = src.substring(1,src.length());
		    log.println("Should shut down input from " + src);
		    owner.removePortlet(src, ps);
		    break;
		case '*':
		    log.println("Should list connections");
		    String desc = owner.describe(this);
		    if (reader.isTextMode()) {
			ps.print(NetType.netString(desc));
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
	    reader.expectBlock(reader.getSize());
	    //protocol.respondToBlock();
	    //protocol.sendAck();
	    protocol.endRead();

	    log.println("Finished");
	}
	protocol.close();

	//clientSocket.close();
	} catch (IOException e) {
	    log.println("Portlet IO shutdown - reason " + e);
	    try {
		if (protocol!=null) {
		    protocol.close();
		}
	    } catch (IOException e2) {
		log.println("Portlet IO shutdown failed - " + e2);
	    }
	    // should call owner to forget me
	}

	log.println("leaving portlet");

	show(getFromName(),
	     "Stopping input from " + getFromName() + " to " + getToName());
	owner.removePortlet(this);

	log.println("left portlet");
    }

    private void show(String from, String msg) {
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

}
