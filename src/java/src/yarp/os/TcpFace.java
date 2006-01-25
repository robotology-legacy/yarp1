
package yarp.os;

import java.io.*;
import java.net.*;

class TcpFace implements Face {
    private static Logger log = Logger.get();
    private Address address = null;
    private ServerSocket serverSocket = null;

    public void open(Address address) throws IOException {
	serverSocket = new ServerSocket(address.getPort());
	this.address = address;
    }

    public InputProtocol read() throws IOException {
	if (serverSocket!=null) {
	    Socket clientSocket = null;
	    clientSocket = serverSocket.accept();
	    log.println("TcpFace got something: " +
			clientSocket.getLocalAddress() + " " +
			clientSocket.getLocalSocketAddress() + " " +
			clientSocket.getRemoteSocketAddress());
	    if (clientSocket!=null) {
		Protocol proto = 
		    new Protocol(new TcpCarrier(clientSocket));
		proto.setTag(true,false);
		return proto;
	    } else {
		throw(new IOException("Failed to read from " + address));
	    }
	}
	return null;
    }

    public void close() throws IOException {
	if (serverSocket!=null) {
	    ServerSocket tmp = serverSocket;
	    serverSocket = null;

	    log.println("tmp connect for shutdown purposes");
	    try {
		Socket sig = new Socket(address.getName(),address.getPort());
		sig.close();
	    } catch (IOException e) {
		log.println("safely ignoring " + e);
	    }
	    log.println("tmp connect for shutdown purposes over");

	    tmp.close();
	}
    }

    public boolean isClosed() {
	return serverSocket == null;
    }
}

