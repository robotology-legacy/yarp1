
package yarp.os;

import java.io.*;
import java.net.*;
import java.nio.channels.*;

class TcpFace implements Face {
    private static Logger log = Logger.get();
    private Address address = null;
    private ServerSocket serverSocket = null;

    public void open(Address address) throws IOException {
	//serverSocket = new ServerSocket(address.getPort());
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

    public Carrier read() throws IOException {
	if (serverSocket!=null) {
	    Socket clientSocket = null;
	    SocketChannel sc = serverSocket.getChannel().accept();
	    if (sc!=null) {
		clientSocket = sc.socket();
		log.println("TcpFace got something");
	    }
	    if (clientSocket!=null) {
		return new TcpCarrier(clientSocket);
	    } else {
		throw(new IOException("Failed to read from " + address));
	    }
	}
	return null;
    }

    public void close() throws IOException {
	if (serverSocket!=null) {
	    serverSocket.close();
	}
    }

}

