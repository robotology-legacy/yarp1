
package yarp.os;

import java.io.*;
import java.util.*;

// no mention of sockets!  keep it this way...

class BasicPort extends Thread implements Port {
    private Address address;
    //private Face face = new TcpNioFace();
    private Face face = null;
    //private Face face = new TcpFace();
    private List connections = 
	Collections.synchronizedList(new ArrayList());
    private List portlets = 
	Collections.synchronizedList(new ArrayList());
    private ProtocolHandler handler = null;
    private String key = null;
    private boolean shutdown = false;
    protected static Logger log = Logger.get();

    public BasicPort(String name) {
	register(name);
    }

    public BasicPort(Address address, String name) {
	this.address = address;
	this.key = name;
    }

    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	this.key = server.getRegName();
	this.address = server;
	log.info("Registered port " + key + " as " + 
		 address);
    }

    public String getPortName() {
	return key;
    }

    public String name() {
	return getPortName();
    }

    public synchronized void setHandler(ProtocolHandler handler) {
	this.handler = handler;
    }

    public synchronized void addPortlet(Portlet portlet) {
	portlets.add(portlet);
    }

    public synchronized void removePortlet(Portlet portlet) {
	portlets.remove(portlet);
    }

    public void println(PrintStream out, String str, boolean info) {
	if (out!=null) {
	    out.println(str);
	}
	if (info) {
	    log.info(str);
	} else {
	    log.debug(str);
	}
    }

    public void println(PrintStream out, String str) {
	println(out,str,false);
    }

    public synchronized void removePortlet(String target, PrintStream out) {
	BasicPortlet rip = null;
	for (Iterator it = portlets.iterator(); it.hasNext(); ) {
	    BasicPortlet p = (BasicPortlet) it.next();
	    String name = p.getRoute().getFromName();
	    if (name!=null) {
		if (name.equals(target)) {
		    rip = p;
		}
	    }
	}
	if (rip!=null) {
	    //log.info("Here i am");
	    println(out,"Removed connection from " + 
		    rip.getRoute().getFromName() +
		    " to " + rip.getRoute().getToName());
	    //log.info("Here i go");
	    rip.close();
	    removePortlet(rip);
	} else {
	    println(out,"Did not find input from " + target);
	}
    }


    public synchronized void addConnection(Connection connection) {
	connections.add(connection);
    }

    public synchronized void removeConnection(String target, PrintStream out) {
	Connection defunct = null;
	for (Iterator it = connections.iterator(); it.hasNext(); ) {
	    Connection connection = (Connection) it.next();
	    if (connection.getFromName().equals(target) ||
		connection.getToName().equals(target)) {
		defunct = connection;
	    }
	}
	if (defunct!=null) {
	    log.debug("closing something that is writing to " +
		      target);
	    println(out, "Stopping output from " +  defunct.getFromName() +
		    " to " + defunct.getToName(),true);
	    defunct.close();
	    connections.remove(defunct);
	} else {
	    println(out, "Could not find " + target);
	}
    }


    public synchronized void handleData(BlockReader reader) throws IOException {
	if (handler!=null) {
	    handler.read(reader);
	}
    }

    public synchronized String describe(Portlet caller) 
	throws IOException {

	StringBuffer buf = new StringBuffer("");

	buf.append("This is " + getPortName() + "\n");

	for (Iterator it = connections.iterator(); it.hasNext(); ) {
	    Connection connection = (Connection) it.next();
	    buf.append("There is a connection from " +
		       connection.getFromName() + " to " +
		       connection.getToName() + " using protocol " +
		       connection.getCarrierName() + "\n");
	}
	if (connections.size()==0) {
	    buf.append("There are no outgoing connections\n");
	}

	for (Iterator it = portlets.iterator(); it.hasNext(); ) {
	    BasicPortlet p = (BasicPortlet) it.next();
	    String conj = "a";
	    if (p == caller) {
		conj = "this";
	    }
	    buf.append("There is " + conj + " connection from " +
		       p.getRoute().getFromName() + " to " +
		       p.getRoute().getToName() + " using protocol " +
		       p.getRoute().getCarrierName() + "\n");
	}
	if (portlets.size()==0) {
	    buf.append("There are no incoming connections\n");
	}
	buf.append("*** end of message\n");

	return buf.toString();
    }

    public void run() {
	boolean finish = false;
	try {
	    //face.open(address);
	    face = Carriers.listen(address);
        } catch (IOException e) {
            log.error("Could not listen on port: " + 
		      address.getPort());
	    finish = true;
        }

	while (!finish) {
	    InputProtocol carrier = null;
	    try {
		if (face!=null) {
		    carrier = face.read();
		}
	    } catch (IOException e) {
		log.info("Shutting down port " + getPortName());
		finish = true;
	    }

	    if (carrier==null) {
		finish = true;
	    }

	    if (shutdown) {
		finish = true;
	    }

	    if (!finish) {
		Portlet portlet;
		try {
		    portlet = newPortlet(carrier);
		    addPortlet(portlet);
		    //portlet.start();
		} catch (IOException e) {
		    log.error(e.toString());
		}
	    }
	}
	log.println("So long, from port " + getPortName());
	try {
	    if (face!=null) {
		face.close();
	    }
	} catch (IOException e) {
	    log.error(e.toString());
	}
    }

    public Portlet newPortlet(InputProtocol proto) throws IOException {
	return new BasicPortlet(this,
				proto);
    }

    public void send(Content content) {
	List defuncts = new LinkedList();
	for (Iterator it = connections.iterator(); it.hasNext(); ) {
	    Connection connection = (Connection) it.next();
	    try {
		// This is a bit unoptimized - the process of generating
		// content in the C++ implementation is done just once
		// and then replicated.
		connection.write(content);
	    } catch (IOException e) {
		defuncts.add(connection);
	    }
	}
	for (Iterator it = defuncts.iterator(); it.hasNext(); ) {
	    Connection connection = (Connection) it.next();
	    log.error("problem with output from " + 
		      connection.getFromName() + " to " +
		      connection.getToName());
	    removeConnection(connection.getToName(),null);
	}
    }

    public synchronized void close() {
	log.println("starting Port close...");
	NameClient.getNameClient().unregister(key);
	shutdown = true;
	try {
	    face.close();
	} catch (IOException e) {
	    log.error(e.toString());
	}
	for (Iterator it = portlets.iterator(); it.hasNext(); ) {
	    Portlet portlet = (Portlet) it.next();
	    portlet.close();
	}
	for (Iterator it = connections.iterator(); it.hasNext(); ) {
	    Connection connection = (Connection) it.next();
	    log.info("stopping output from " + connection.getFromName() +
		     " to " + connection.getToName());
	    connection.close();
	}

	log.println("ending Port close...");
	portlets.clear();
	interrupt();
    }


    public void connect(String src) throws IOException {
	connect(src,null);
    }

    public void connect(String src, PrintStream ps) throws IOException {
	BasicPort owner = this;
	String ownerName = getPortName();

	String srcName = NameClient.getNamePart(src);
	String srcCarrier = NameClient.getProtocolPart(src);
	Address add = NameClient.getNameClient().query(srcName);

	if (add==null) {
	    //throw(new IOException());
	    owner.println(ps,"Cannot find " + srcName,true);
	    return;
	}
	if (add.getRegName()!=null) {
	    srcName = add.getRegName();
	}
	owner.removeConnection(srcName,null);
	String msg = "Sending output from " + 
	    ownerName + " to " +
	    srcName + " using " + srcCarrier;
	if (add!=null) {
	    // MAY need a hack for YARP1 compatibility
	    //add = new Address(add.getName(), add.getPort(),
	    //	      srcCarrier);
	    if (NameClient.canConnect(ownerName,srcName,srcCarrier)) {
		owner.addConnection(new Connection(add,srcCarrier,owner.getPortName(),srcName,true));
		owner.println(ps,msg,true);
	    } else {
		owner.println(ps,"ports cannot be connected using that protocol",true);
	    }
	} else {
	    owner.println(ps,"Cannot find " + srcName,true);
	}
    }

}
