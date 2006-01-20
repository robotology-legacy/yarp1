
package yarp.os;

import java.io.*;
import java.net.*;

class TcpCarrier extends AbstractCarrier {

    protected TwoWayStream way = null;
    private boolean requireAck = true;

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

    public boolean checkHeader(byte[] header) {
	return getSpecifier(header)%16 == getSpecifier();
    }

    public byte[] getHeader() {
	// always request ack
	return createStandardHeader(getSpecifier()+(requireAck?128:0));
    }

    public void setRequireAck(boolean flag) {
	requireAck = flag;
    }

    public void setParameters(byte[] header) {
	int specifier = getSpecifier(header);
	requireAck = (specifier&128)!=0;
	log.println("require ack is " + requireAck);
    }

    public boolean requireAck() {
	return requireAck;
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
	int cport = proto.getStreams().getLocalAddress().getPort();
	log.println("setting port number to " + cport);
	proto.writeYarpInt(cport);
	return true;
    }

    public TwoWayStream takeStreams() {
	log.println("TcpCarrier giving away socket");
	TwoWayStream r = way;
	way = null;
	return r;
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

