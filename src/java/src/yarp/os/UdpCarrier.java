
package yarp.os;

import java.io.*;
import java.net.*;

class UdpCarrier extends Carrier {
    DatagramSocket dgram;

    public String getName() {
	return "udp";
    }

    public int getSpecifier() {
	return 97;
    }

    public void sendExtraHeader(Protocol proto) throws IOException {
	System.out.println("UDP sendExtraHeader");
    }


    public void expectReplyToHeader(Protocol proto) throws IOException {
	int port = readPort(proto);
	proto.become("udp",new Address("ignore",port));
    }

    public void respondExtraToHeader(Protocol proto) throws IOException {
	System.out.println("respondExtraToHeader for " + getName());
	proto.become(getName(),null);
    }

    public void close() throws IOException {
    }

    public void open(Address address, Carrier previous) throws IOException {
	int remotePort = -1;
	if (address!=null) {
	}
    }
}

