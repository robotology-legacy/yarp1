
package yarp.os;

import java.io.*;

// no mention of sockets!  keep it this way...

class BasicPortlet implements Portlet, ProtocolHandler {
    private Logger log = Logger.get();
    private InputProtocol proto = null;

    private InputConnection con = null;

    private BasicPort owner;
    private PrintStream ps = null;
    private PrintStream reply = null;
    private CommandContent cmdContent = new CommandContent();


    public BasicPortlet(BasicPort owner, InputProtocol proto) {
	this.owner = owner;
	this.proto = proto;
	String ownerName = owner.getPortName();
	log.println("BasicPortlet for " + ownerName + " starting up");
	log = new Logger(ownerName,Logger.get());
	try {
	    reply = new PrintStream(proto.getReplyStream());
	    con = new InputConnection(proto,ownerName,this);
	} catch (IOException e) {
	    log.error("Connection failed: " + e);
	}

	show(getRoute().getFromName(), 
	     "Receiving input from " + getRoute().getFromName() + " to " + 
	     getRoute().getToName() + " using " + getRoute().getCarrierName());

	con.start();
    }


    public synchronized boolean read(BlockReader reader) throws IOException {
	String sourceName = getRoute().getFromName();
	String ownerName = getRoute().getToName();
	String carrierName = getRoute().getCarrierName();
	if (reader==null) {
	    // this is a shutdown request
	    log.println("leaving portlet");
	    
	    owner.removePortlet(this);
	    
	    log.println("left portlet");
	    return true;
	}

	boolean done = false;

	if (reader.isTextMode()) {
	    ps = reply;
	}
	
	cmdContent.read(reader);

	int cmd = cmdContent.getKey();
	String src = cmdContent.getText();
	
	if (cmd!=0) {
	    log.println("Got command [" + ((char)cmd) + "]");
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
		log.println(proto.getRoute().getFromName() + 
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
		    log.println("DESCRIPTION:\n" + desc);
		    ps.print(desc);
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

	return done;
    }

    public synchronized void close() {
	log.println("Trying to halt portlet");

	show(getRoute().getFromName(),
	     "Stopping input from " + getRoute().getFromName() + " to " + 
	     getRoute().getToName());


	try {
	    //read(null);

	    if (proto!=null) {
		proto.close();
	    }
	} catch (IOException e) {
	    log.println("Problem while halting portlet");
	}

	log.println("Progress halting portlet");
	con.interrupt();
	log.println("Done halting portlet");
    }

    public Route getRoute() {
	return con.getRoute();
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
