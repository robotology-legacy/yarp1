
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;

public class Port {
    private Address address;
    private ServerSocket serverSocket = null;
    private ArrayList connections = new ArrayList();
    private ArrayList portlets = new ArrayList();

    public Port(Address address) {
	this.address = address;
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

    public synchronized Reader getReader() {
	return null;
    }

    public void run() throws IOException {
	try {
            serverSocket = new ServerSocket(address.getPort());
        } catch (IOException e) {
            System.err.println("Could not listen on port: " + 
			       address.getPort());
            System.exit(1);
        }

	while (true) {
	    Socket clientSocket = null;
	    try {
		clientSocket = serverSocket.accept();
	    } catch (IOException e) {
		System.err.println("Accept failed.");
		System.exit(1);
	    }
	    System.out.println("got something");
	    
	    Portlet portlet = new Portlet(this,
					  new SocketShiftStream(clientSocket));
	    addPortlet(portlet);
	    portlet.start();
	}
    }
}
