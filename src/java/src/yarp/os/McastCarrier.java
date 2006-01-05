
package yarp.os;

import java.io.*;
import java.net.*;
import java.nio.channels.*;
import java.util.*;

class McastCarrier extends Carrier {
    private String senderName;
    private Address mcastAddress;
    MulticastSocket mcast;
    InputStream is;
    OutputStream os;

    public String getName() {
	return "mcast";
    }

    public int getSpecifier() {
	return 98;
    }

    public void sendExtraHeader(Protocol proto) throws IOException {
	System.out.println("sendExtraHeader for " + getName());

	System.out.println("Sending mcast info");
	byte b[] = new byte[6];

	InetAddress inet = InetAddress.getByName(mcastAddress.getName());
	byte[] raw = inet.getAddress();
	if (raw.length!=4) {
	    System.err.println("address strange size");
	    System.exit(1);
	}
	for (int i=0; i<4; i++) {
	    b[i] = raw[i];
	}
	int port = mcastAddress.getPort();
	b[5] = (byte)(port%256);
	b[4] = (byte)(port/256);
	
	proto.write(b);
    }

    public Address mcastQuery(String name) {
	NameClient nc = NameClient.getNameClient();
	String q = nc.getNamePart(name) + "-mcast";
        Address address = nc.probe("NAME_SERVER query " + q);
	if (address==null) {
	    return nc.register(nc.getNamePart(name),"mcast");
	}
	return address;
    }

    public void prepareSend(Protocol proto) {
	System.out.println("prepareSend for " + getName());
	senderName = proto.getSender();
	System.out.println("Getting mcast info");
	mcastAddress = mcastQuery(senderName);
	System.out.println("mcast address is " + mcastAddress);
    }

    public void expectExtraHeader(Protocol proto) throws IOException {
	System.out.println("expectExtraHeader for " + getName());
	System.out.println("Looking for mcast info");
	byte b[] = new byte[6];
	proto.read(b);
	int ip[] = new int[4];
	for (int i=0; i<4; i++) {
	    ip[i] = NetType.unsigned(b[i]);
	    System.out.println("  >> IP " + ip[i]);
	}
	// strange byte order
	int port = NetType.netInt(new byte[] { b[5], b[4], 0, 0 });
	System.out.println("  >> PORT " + port);
	
	mcastAddress = new Address("" + ip[0] + "." + 
				   ip[1] + "." +
				   ip[2] + "." +
				   ip[3],
				   port);
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	// in mcast case, don't expect port number
	// super.expectReplyToHeader(proto);
	proto.become(getName(),mcastAddress);
    }

    public void respondExtraToHeader(Protocol proto) throws IOException {
	System.out.println("respondExtraToHeader for " + getName());
	proto.become(getName(),mcastAddress);
    }

    public void close() throws IOException {
	if (mcast!=null) {
	    mcast.close();
	    mcast = null;
	}
    }

    public void open(Address address, Carrier previous) throws IOException {
	close();
	Address local = previous.getLocalAddress();
	Address remote = previous.getLocalAddress();
	remote = address;
	InetAddress group = InetAddress.getByName(address.getName());
	mcast = new MulticastSocket(address.getPort());
	mcast.joinGroup(group);
	setAddress(local,remote);
	is = new DatagramInputStream(mcast,512);
	os = new BufferedOutputStream(new DatagramOutputStream(mcast,
							       remote,
							       512),
				      512);
    }


    public InputStream getInputStream() throws IOException {
	return is;
    }

    public OutputStream getOutputStream() throws IOException {
	return os;
    }

}

