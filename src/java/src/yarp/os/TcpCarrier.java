
package yarp.os;

import java.io.*;
import java.net.*;

class TcpCarrier extends Carrier {
    Socket socket;
    //Address local;
    //Address remote;

    public String getName() {
	return "tcp";
    }

    public int getSpecifier() {
	return 3;
	//return 0x64; // second byte: 0x1e
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	log.println("expectReplyToHeader for tcp");
	readPort(proto);
	// For TCP, a delay seems to be needed when talking to C++ yarp - 
	// anyone know why?
	//Time.delay(2);  -- this is needed for C++ yarp
    }

    /*
    public Address getLocalAddress() {
	return local;
    }

    public Address getRemoteAddress() {
	return remote;
    }
    */

    public void close() throws IOException {
	log.println("TcpCarrier.close");
	if (socket!=null) {
	    log.println("TcpCarrier.close socket");
	    socket.close();
	    socket = null;
	}
    }

    public Socket takeSocket() {
	log.println("TcpCarrier giving away socket");
	Socket r = socket;
	socket = null;
	return r;
    }

    public void open(Address address, ShiftStream previous) throws IOException {
	socket = previous.takeSocket();
	InetAddress a1 = socket.getLocalAddress();
	Address clocal = 
	    new Address(a1.getHostAddress(),socket.getLocalPort());
	InetSocketAddress a2 = 
	    (InetSocketAddress)socket.getRemoteSocketAddress();
	Address cremote = new Address(a2.getHostName(),socket.getPort());
	setAddress(clocal,cremote);
    }


    public TcpCarrier() {
    }

    public void start(Address address) throws IOException {
	try {
	    socket = new Socket(address.getName(),address.getPort());
	    start(socket);
	} catch (UnknownHostException e) {
	    throw(new IOException("Don't know about host " + 
				  address.getName()));    
	}
    }

    public TcpCarrier(Socket socket) throws IOException {
	start(socket);
    }

    public void start(Socket socket) throws IOException {
	this.socket = socket;
	InetAddress a1 = socket.getLocalAddress();
	Address clocal = 
	    new Address(a1.getHostAddress(),socket.getLocalPort());
	InetSocketAddress a2 = 
	    (InetSocketAddress)socket.getRemoteSocketAddress();
	Address cremote = new Address(a2.getHostName(),socket.getPort());
	setAddress(clocal,cremote);
    }


    public InputStream getInputStream() throws IOException {
	return socket.getInputStream();
    }

    public OutputStream getOutputStream() throws IOException {
	return socket.getOutputStream();
    }

    public Carrier create() {
	log.println("*** TcpCarrier::create()");
	return new TcpCarrier();
    }

    public boolean isConnectionless() throws IOException {
	return false;
    }
}

