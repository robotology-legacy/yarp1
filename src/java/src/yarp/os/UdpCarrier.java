
package yarp.os;

import java.io.*;
import java.net.*;

class UdpCarrier extends Carrier {
    DatagramSocket dgram;
    InputStream is;
    OutputStream os;
    boolean reading = false;

    public String getName() {
	return "udp";
    }

    public int getSpecifier() {
	return 0;
	//return 97;
	//return 0x61;
    }

    //public void sendExtraHeader(Protocol proto) throws IOException {
    //log.println("UDP sendExtraHeader");
    //}


    public void expectReplyToHeader(Protocol proto) throws IOException {
	int port = readPort(proto);
	//proto.become("udp",new Address("ignore",port));
	proto.become(this,new Address("ignore",port));
    }

    public boolean respondToHeader(Protocol proto) throws IOException {
	super.respondToHeader(proto);
	log.println("respondExtraToHeader for " + getName());
	reading = true;
	//proto.become(getName(),null);
	proto.become(this,null);
	return true;
    }

    public void close() throws IOException {
	log.println("UdpCarrier.close");
	if (dgram!=null) {
	    log.println("UdpCarrier.close socket");
	    dgram.close();
	    dgram = null;
	}
    }

    public void open(Address address, ShiftStream previous) throws IOException {
	close();
	Address clocal = previous.getLocalAddress();
	Address cremote = previous.getRemoteAddress();
	dgram = new DatagramSocket(clocal.getPort());
	if (address!=null) {
	    cremote = new Address(cremote.getName(),address.getPort());
	}
	setAddress(clocal,cremote);
	is = new DatagramInputStream(dgram,512);
	os = new BufferedOutputStream(new DatagramOutputStream(dgram,
							       cremote,
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

    public Carrier create() {
	return new UdpCarrier();
    }
}

