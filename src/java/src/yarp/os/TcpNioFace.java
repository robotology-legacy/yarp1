
package yarp.os;

import java.io.*;
import java.net.*;
import java.nio.channels.*;

/**
 * Same as TcpFace, but using the nio library
 */

class TcpNioFace implements Face {
    private static Logger log = Logger.get();
    private Address address = null;
    private ServerSocket serverSocket = null;

    public void open(Address address) throws IOException {
	ServerSocketChannel sc = ServerSocketChannel.open();
	if (sc!=null) {
	    serverSocket = sc.socket();
	    SocketAddress a = new InetSocketAddress(address.getPort());
	    serverSocket.bind(a);
	    this.address = address;
	} else {
	    throw(new IOException("failed to listen to tcp at address: " + 
				  address));
	}
    }

    public InputProtocol read() throws IOException {
	if (serverSocket!=null) {
	    Socket clientSocket = null;
	    SocketChannel sc = serverSocket.getChannel().accept();
	    if (isClosed()) {
		return null;
	    }
	    if (sc!=null) {
		clientSocket = sc.socket();
		log.println("TcpNioFace got something: " +
			    clientSocket.getLocalAddress() + " " +
			    clientSocket.getRemoteSocketAddress());
	    }
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
	    tmp.close();
	}
    }

    public boolean isClosed() {
	return serverSocket == null;
    }
}

