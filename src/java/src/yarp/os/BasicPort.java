
package yarp.os;

import java.io.*;
import java.nio.channels.*;
import java.net.*;
import java.util.*;

class BasicPort extends Thread implements Port {
    private Address address;
    private ServerSocket serverSocket = null;
    private List connections = 
	Collections.synchronizedList(new ArrayList());
    private List portlets = 
	Collections.synchronizedList(new ArrayList());
    private ProtocolHandler handler = null;
    private String key = null;
    private boolean shutdown = false;
    private static Logger log = Logger.get();

    public BasicPort(String name) {
	register(name);
    }

    public BasicPort(Address address, String name) {
	this.address = address;
	this.key = name;
    }

    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	log.info("Registered port " + name + " as " + 
		 server.toString());
	this.address = server;
	this.key = name;
    }

    public String getPortName() {
	return key;
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
	    String name = p.getFromName();
	    if (name!=null) {
		if (name.equals(target)) {
		    rip = p;
		}
	    }
	}
	if (rip!=null) {
	    //log.info("Here i am");
	    println(out,"Removed connection from " + rip.getFromName() +
		    " to " + rip.getToName());
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
	    /*
	    log.error("could not find " +
		      target + " (" + target.length() + " chars) in order to close it");
	    for (Iterator it = connections.iterator(); it.hasNext(); ) {
		Connection connection = (Connection) it.next();
		log.println("  Found instead "+connection.getFromName()
			    + " to " + connection.getToName());
	    }
	    */
	}
    }


    public synchronized void handleData(Protocol protocol) throws IOException {
	if (handler!=null) {
	    handler.read(protocol);
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
		       p.getFromName() + " to " +
		       p.getToName() + " using protocol " +
		       p.getCarrierName() + "\n");
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
            //serverSocket = new ServerSocket(address.getPort());
            //serverSocket = new ServerSocket(address.getPort());
	    ServerSocketChannel sc = ServerSocketChannel.open();
	    if (sc!=null) {
		serverSocket = sc.socket();
		SocketAddress a = new InetSocketAddress(address.getPort());
		serverSocket.bind(a);
	    }
        } catch (IOException e) {
            log.error("Could not listen on port: " + 
		      address.getPort());
	    finish = true;
        }

	while (!finish) {
	    Socket clientSocket = null;
	    try {
		SocketChannel sc = serverSocket.getChannel().accept();
		if (sc!=null) {
		    clientSocket = sc.socket();
		    log.println("got something");
		    /*
		    if (clientSocket.getLocalPort()==-1) {
			log.error("problem, possibly with old gcj version");
			System.exit(1);
		    }
		    */
		
		}
	    } catch (IOException e) {
		log.info("Shutting down port " + getPortName());
		finish = true;
	    }

	    if (shutdown) {
		finish = true;
	    }

	    if (!finish) {
		Portlet portlet;
		try {
		    portlet = newPortlet(clientSocket);
		    addPortlet(portlet);
		    portlet.start();
		} catch (IOException e) {
		    log.error(e.toString());
		}
	    }
	}
	log.info("So long, from port " + getPortName());
    }

    public Portlet newPortlet(Socket socket) throws IOException {
	return new BasicPortlet(this,
				new TcpCarrier(socket));
	//return new BasicPortlet(this,
	//		new CarrierShiftStream(socket));
    }

    public void send(Content content) {
	List defuncts = new LinkedList();
	for (Iterator it = connections.iterator(); it.hasNext(); ) {
	    Connection connection = (Connection) it.next();
	    try {
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
	shutdown = true;
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
	    throw(new IOException());
	}
	if (add.getRegName()!=null) {
	    srcName = add.getRegName();
	}
	owner.removeConnection(srcName,null);
	String msg = "Sending output from " + 
	    ownerName + " to " +
	    srcName + " using " + srcCarrier;
	if (add!=null) {
	    add = new Address(add.getName(), add.getPort(),
			      srcCarrier);
	    if (NameClient.canConnect(ownerName,srcName,srcCarrier)) {
		owner.addConnection(new Connection(add,owner.getPortName(),srcName));
		owner.println(ps,msg,true);
	    } else {
		owner.println(ps,"ports cannot be connected using that protocol",true);
	    }
	} else {
	    owner.println(ps,"Cannot find " + srcName,true);
	}
    }



}
