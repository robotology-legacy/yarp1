
package yarp.os;

import java.io.*;

abstract class Carrier {
    Address local, remote;

    public abstract String getName();
    public abstract int getSpecifier();

    public Address getLocalAddress() {
	return local;
    }

    public Address getRemoteAddress() {
	return remote;
    }

    public void setAddress(Address local, Address remote) {
	this.local = local;
	this.remote = remote;
    }

    public void sendExtraHeader(Protocol proto) throws IOException {
	System.out.println("sendExtraHeader for " + getName());
    }

    public void prepareSend(Protocol proto) {
	System.out.println("prepareSend for " + getName());
    }

    public void expectExtraHeader(Protocol proto) throws IOException {
	System.out.println("expectExtraHeader for " + getName());
    }

    public int readPort(Protocol proto) throws IOException {
	int port = 0;
	byte b[] = new byte[8];
	proto.read(b);
	//System.out.println(b[0]=='Y');
	port = NetType.unsigned(b[2])+256*NetType.unsigned(b[3]);
	System.out.println("Port number is " + port);
	if (b[0]!='Y') {
	    throw new IOException();
	}
	return port;
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	System.out.println("expectReplyToHeader for " + getName());
    }
    
    public void respondExtraToHeader(Protocol proto) throws IOException {
	System.out.println("respondExtraToHeader for " + getName());
    }

    public void close() throws IOException {
    }

    public void open(Address address, Carrier previous) throws IOException {
    }

    public abstract InputStream getInputStream() throws IOException;
    public abstract OutputStream getOutputStream() throws IOException;
}
