
package yarp.os;

import java.io.*;
import java.net.*;

class DatagramTwoWayStream implements TwoWayStream {
    private DatagramSocket socket;
    private Address local, remote;
    private InputStream is;
    private OutputStream os;
    private boolean reading = false;

    DatagramTwoWayStream(DatagramSocket socket, boolean reading) {
	this.socket = socket;
	this.reading = reading;
	InetAddress a1 = socket.getLocalAddress();
	local = new Address(a1.getHostAddress(),socket.getLocalPort());

	InetSocketAddress a2 = 
	    (InetSocketAddress)socket.getRemoteSocketAddress();
	remote = new Address(a2.getHostName(),socket.getPort()); 
	createStreams();
   }

    DatagramTwoWayStream(DatagramSocket socket, Address local, Address remote,
		       boolean reading) {
	this.socket = socket;
	this.reading = reading;
	this.local = local;
	this.remote = remote;
	createStreams();
    }

    void createStreams() {
	is = new DatagramInputStream(socket,512);
	os = new BufferedOutputStream(new DatagramOutputStream(socket,
							       remote,
							       512,
							       reading),
				      512);
    }

    public InputStream getInputStream() throws IOException {
	return is;
    }

    public OutputStream getOutputStream() throws IOException { 
	return os;
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
	    is = null;
	    os = null;
	}
    }

    public DatagramSocket get() {
	return socket;
    }
}
