
package yarp.os;

import java.io.*;
import java.net.*;

class UdpCarrier extends Carrier {
    TwoWayStream way = null;

    public TwoWayStream getStreams() {
	return way;
    }
	
    boolean reading = false;

    public String getName() {
	return "udp";
    }

    public int getSpecifier() {
	return 0;
	//return 97;
	//return 0x61;
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	int port = readPort(proto);
	//proto.become("udp",new Address("ignore",port));
	proto.become(this,new Address("ignore",port));
    }

    public boolean respondToHeader(Protocol proto) throws IOException {
	log.println("respondToHeader for " + getName());
	makeDgram(null,getRemoteAddress());
	//makeDgram(getLocalAddress());
	log.println("local address is now " + getLocalAddress());

	int cport = getLocalAddress().getPort();
	log.println("setting port number to " + cport);
	proto.writeYarpInt(cport);
	reading = true;
	//proto.become(getName(),null);
	proto.become(this,null);
	return true;
    }

    public void close() throws IOException {
	log.println("UdpCarrier.close");
	if (way!=null) {
	    log.println("UdpCarrier.close socket");
	    way.close();
	    way = null;
	}
    }

    public Address makeDgram(Address address,Address remote) 
	throws IOException {
	if (way == null) {
	    DatagramSocket dgram;
	    if (address==null) {
		dgram = new DatagramSocket();
	    } else {
		dgram = new DatagramSocket(address.getPort());
	    }

	    Address clocal = 
		new Address(NameClient.getNameClient().getHostName(),
			    dgram.getLocalPort());
	    //setAddress(clocal,getRemoteAddress());
	    way = new DatagramTwoWayStream(dgram,clocal,remote,reading);
	}
	return getLocalAddress();
    }

    public void open(Address address, ShiftStream previous) throws IOException {
	Address clocal = previous.getLocalAddress();
	Address cremote = previous.getRemoteAddress();

	previous.close();

	if (address!=null) {
	    cremote = new Address(cremote.getName(),address.getPort());
	}
	clocal = makeDgram(clocal,cremote);
    }

    public Carrier create() {
	return new UdpCarrier();
    }
}

