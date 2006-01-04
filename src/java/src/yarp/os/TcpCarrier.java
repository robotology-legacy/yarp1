
package yarp.os;

import java.io.*;
import java.net.*;

class TcpCarrier extends Carrier {
    Socket socket;
    Address local;
    Address remote;

    public String getName() {
	return "tcp";
    }

    public int getSpecifier() {
	return 0x64; // second byte: 0x1e
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	readPort(proto);
	// For TCP, a delay seems to be needed when talking to C++ yarp - 
	// anyone know why?
	Time.delay(2);
    }

    public Address getLocalAddress() {
	return local;
    }

    public Address getRemoteAddress() {
	return remote;
    }

    public void close() throws IOException {
	if (socket!=null) {
	    socket.close();
	    socket = null;
	}
    }

    public void open(Address address, Carrier previous) throws IOException {
	System.out.println("Cannot open TCP; happens externally");
	System.exit(1);
    }

    public void open(Socket socket) throws IOException {
	this.socket = socket;
	InetAddress a1 = socket.getLocalAddress();
	local = new Address(a1.getHostAddress(),socket.getLocalPort());
	InetSocketAddress a2 = 
	    (InetSocketAddress)socket.getRemoteSocketAddress();
	remote = new Address(a2.getHostName(),socket.getPort());
    }

}

