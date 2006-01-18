
package yarp.os;

import java.io.*;
import java.net.*;

class TcpCarrier extends Carrier {

    protected TwoWayStream way = null;

    public TwoWayStream getStreams() {
	return way;
    }

    public String getName() {
	return "tcp";
    }

    public int getSpecifier() {
	return 3;
	//return 0x64; // second byte: 0x1e
    }



    public TcpCarrier() {
    }

    public TcpCarrier(Socket socket) throws IOException {
	start(socket);
    }

    public void start(Address address) throws IOException {
	try {
	    Socket socket = new Socket(address.getName(),address.getPort());
	    start(socket);
	} catch (UnknownHostException e) {
	    throw(new IOException("Don't know about host " + 
				  address.getName()));    
	}
    }

    public void start(Socket socket) throws IOException {
	/*
	this.socket = socket;
	InetAddress a1 = socket.getLocalAddress();
	Address clocal = 
	    new Address(a1.getHostAddress(),socket.getLocalPort());
	InetSocketAddress a2 = 
	    (InetSocketAddress)socket.getRemoteSocketAddress();
	Address cremote = new Address(a2.getHostName(),socket.getPort());
	setAddress(clocal,cremote);
	*/
	log.println("*** TcpCarrier started with socket");
	way = new SocketTwoWayStream(socket);
    }


    public void expectReplyToHeader(Protocol proto) throws IOException {
	log.println("expectReplyToHeader for tcp");
	readPort(proto);
	// For TCP, a delay seems to be needed when talking to C++ yarp - 
	// anyone know why?
	//Time.delay(2);  -- this is needed for C++ yarp
    }

    public void close() throws IOException {
	log.println("TcpCarrier.close");
	if (way!=null) {
	    log.println("TcpCarrier.close socket");
	    way.close();
	    way = null;
	}
    }

    public TwoWayStream takeStreams() {
	log.println("TcpCarrier giving away socket");
	TwoWayStream r = way;
	way = null;
	return way;
    }

    public void open(Address address, ShiftStream previous) 
	throws IOException {
	way = previous.takeStreams();
	/*
	InetAddress a1 = socket.getLocalAddress();
	Address clocal = 
	    new Address(a1.getHostAddress(),socket.getLocalPort());
	InetSocketAddress a2 = 
	    (InetSocketAddress)socket.getRemoteSocketAddress();
	Address cremote = new Address(a2.getHostName(),socket.getPort());
	setAddress(clocal,cremote);
	*/
    }



    public Carrier create() {
	log.println("*** TcpCarrier::create()");
	return new TcpCarrier();
    }

    public boolean isConnectionless() throws IOException {
	return false;
    }
}

