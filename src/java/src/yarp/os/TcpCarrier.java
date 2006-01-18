
package yarp.os;

import java.io.*;
import java.net.*;

class TcpCarrier extends AbstractCarrier {

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
	log.println("*** TcpCarrier started with socket");
	way = new SocketTwoWayStream(socket);
    }


    public void expectReplyToHeader(Protocol proto) throws IOException {
	log.println("expectReplyToHeader for tcp");
	proto.readYarpInt();
    }

    public void close() throws IOException {
	log.println("TcpCarrier.close");
	if (way!=null) {
	    log.println("TcpCarrier.close socket");
	    way.close();
	    way = null;
	}
    }

    public boolean respondToHeader(Protocol proto) throws IOException {
	int cport = proto.getLocalAddress().getPort();
	log.println("setting port number to " + cport);
	proto.writeYarpInt(cport);
	return true;
    }

    public TwoWayStream takeStreams() {
	log.println("TcpCarrier giving away socket");
	TwoWayStream r = way;
	way = null;
	return way;
    }

    public void open(Address address, ShiftStream previous) throws IOException{
	way = previous.takeStreams();
    }

    public Carrier create() {
	log.println("*** TcpCarrier::create()");
	return new TcpCarrier();
    }

    public boolean isConnectionless() {
	return false;
    }
}

