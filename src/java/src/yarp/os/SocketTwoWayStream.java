
package yarp.os;

import java.io.*;
import java.net.*;

class SocketTwoWayStream implements TwoWayStream {
    private Socket socket;
    private Address local, remote;

    SocketTwoWayStream(Socket socket) {
	this.socket = socket;
	InetAddress a1 = socket.getLocalAddress();
	local = new Address(a1.getHostAddress(),socket.getLocalPort());

	InetSocketAddress a2 = 
	    (InetSocketAddress)socket.getRemoteSocketAddress();
	remote = new Address(a2.getHostName(),socket.getPort());
    }

    SocketTwoWayStream(Socket socket, Address local, Address remote) {
	this.socket = socket;
	this.local = local;
	this.remote = remote;
    }

    public InputStream getInputStream() throws IOException {
	return socket.getInputStream();
    }

    public OutputStream getOutputStream() throws IOException { 
	return socket.getOutputStream();
    }

    public Address getLocalAddress() throws IOException {
	return local;
    }

    public Address getRemoteAddress() throws IOException {
	return remote;
    }

    public void close() throws IOException {
	if (socket!=null) {
	    socket.close();
	    socket = null;
	    local = null;
	    remote = null;
	}
    }
}
