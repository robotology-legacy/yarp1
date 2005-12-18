
package yarp.os;

import java.io.*;
import java.net.*;

public class SocketShiftStream implements ShiftStream {
    Socket socket;
    DatagramSocket dgram;
    Address local;
    Address remote;

    public SocketShiftStream(Socket socket) {
	this.socket = socket;
	InetAddress a1 = socket.getLocalAddress();
	local = new Address(a1.getHostAddress(),socket.getLocalPort());
	InetSocketAddress a2 = (InetSocketAddress)socket.getRemoteSocketAddress();
	remote = new Address(a2.getHostName(),socket.getPort());
    }

    public void close() throws IOException {
	if (socket!=null) socket.close();
	if (dgram!=null) dgram.close();
    }

    public InputStream getInputStream() throws IOException {
	if (dgram!=null) {
	    InputStream is = new DatagramInputStream(dgram,512);
	    return is;
	}
	return socket.getInputStream();
    }

    public OutputStream getOutputStream() throws IOException {
	if (dgram!=null) {
	    OutputStream os = new DatagramOutputStream(dgram,
						       remote,
						       512);
	    return new BufferedOutputStream(os,512);
	}
	return socket.getOutputStream();
    }

    public void becomeUdp(int remotePort) throws IOException {
	if (socket!=null) {
	    socket.close();
	    socket = null;
	    dgram = new DatagramSocket(local.getPort());
	}
	if (remotePort>=0) {
	    remote = new Address(remote.getName(),remotePort);
	}
    }


    public int getLocalPort() throws IOException {
	return local.getPort();
    }
}

