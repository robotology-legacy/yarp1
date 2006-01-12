
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

    public void sendExtraHeader(Protocol proto) throws IOException {
	log.println("UDP sendExtraHeader");
    }


    public void expectReplyToHeader(Protocol proto) throws IOException {
	int port = readPort(proto);
	proto.become("udp",new Address("ignore",port));
    }

    public void respondExtraToHeader(Protocol proto) throws IOException {
	log.println("respondExtraToHeader for " + getName());
	reading = true;
	proto.become(getName(),null);
    }

    public void close() throws IOException {
	if (dgram!=null) {
	    dgram.close();
	    dgram = null;
	}
    }

    public void open(Address address, Carrier previous) throws IOException {
	close();
	Address local = previous.getLocalAddress();
	Address remote = previous.getRemoteAddress();
	dgram = new DatagramSocket(local.getPort());
	if (address!=null) {
	    remote = new Address(remote.getName(),address.getPort());
	}
	setAddress(local,remote);
	is = new DatagramInputStream(dgram,512);
	os = new BufferedOutputStream(new DatagramOutputStream(dgram,
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

    public Carrier create() {
	return new UdpCarrier();
    }
}

