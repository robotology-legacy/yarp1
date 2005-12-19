
package yarp.os;

import java.io.*;
import java.nio.channels.*;
import java.net.*;
import java.util.*;

public class Port extends Thread {
    private Address address;
    private ServerSocket serverSocket = null;
    private ArrayList connections = new ArrayList();
    private ArrayList portlets = new ArrayList();
    private ProtocolHandler handler = null;
    private String key = null;

    public Port(Address address, String key) {
	this.address = address;
	this.key = key;
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

    public synchronized void addConnection(Connection connection) {
	connections.add(connection);
    }

    public synchronized void handleData(Protocol protocol) {
	if (handler!=null) {
	    handler.read(protocol);
	}
    }


    public String getPortName() {
	return key;
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
            System.err.println("Could not listen on port: " + 
			       address.getPort());
	    finish = true;
        }

	while (!finish) {
	    Socket clientSocket = null;
	    try {
		SocketChannel sc = serverSocket.getChannel().accept();
		if (sc!=null) {
		    clientSocket = sc.socket();
		}
	    } catch (IOException e) {
		System.err.println("shutting down port.");
		finish = true;
	    }

	    if (!finish) {
		System.out.println("got something");
		
		Portlet portlet = 
		    new Portlet(this,
				new SocketShiftStream(clientSocket));
		addPortlet(portlet);
		portlet.start();
	    }
	}
    }

    public void send(Content content) {
	for (Iterator it = connections.iterator(); it.hasNext(); ) {
	    Connection connection = (Connection) it.next();
	    try {
		connection.send(content);
	    } catch (IOException e) {
		System.err.println("connection problem");
	    }
	}
    }

    public void close() {
	System.out.println("starting Port close...");
	for (Iterator it = portlets.iterator(); it.hasNext(); ) {
	    Portlet portlet = (Portlet) it.next();
	    portlet.close();
	}
	System.out.println("ending Port close...");
	portlets.clear();
	interrupt();
    }
}
