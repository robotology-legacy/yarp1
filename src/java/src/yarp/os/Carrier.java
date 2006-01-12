
package yarp.os;

import java.io.*;

abstract class Carrier {
    private Address local, remote;
    protected static Logger log = Logger.get();

    public abstract String getName();
    public abstract int getSpecifier();

    public Address getLocalAddress() {
	return local;
    }

    public Address getRemoteAddress() {
	return remote;
    }

    public void setAddress(Address local, Address remote) {
	log.println("set local address " + local + " and remote " + remote);
	this.local = local;
	this.remote = remote;
    }

    public void sendExtraHeader(Protocol proto) throws IOException {
	log.println("sendExtraHeader for " + getName());
    }

    public void prepareSend(Protocol proto) {
	log.println("prepareSend for " + getName());
    }

    public void expectExtraHeader(Protocol proto) throws IOException {
	log.println("expectExtraHeader for " + getName());
    }

    public int readPort(Protocol proto) throws IOException {
	int port = 0;
	byte b[] = new byte[8];
	proto.read(b);
	//System.out.println(b[0]=='Y');
	port = NetType.unsigned(b[2])+256*NetType.unsigned(b[3]);
	log.println("Port number is " + port);
	if (b[0]!='Y') {
	    throw new IOException();
	}
	return port;
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	log.println("expectReplyToHeader for " + getName());
    }
    
    public void respondExtraToHeader(Protocol proto) throws IOException {
	log.println("respondExtraToHeader for " + getName());
    }

    public void close() throws IOException {
	log.println("default Carrier close()");
    }

    public void open(Address address, Carrier previous) throws IOException {
    }

    public boolean isActive() throws IOException {
	return true;
    }

    public boolean isConnectionless() throws IOException {
	return true;
    }

    public abstract InputStream getInputStream() throws IOException;
    public abstract OutputStream getOutputStream() throws IOException;

    public abstract Carrier create();
}
