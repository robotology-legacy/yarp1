
package yarp.os;

import java.io.*;

abstract class Carrier implements ShiftStream {

    protected static Logger log = Logger.get();


    // things that must be specified

    public abstract String getName();

    public abstract int getSpecifier();

    public abstract void open(Address address, ShiftStream previous) 
	throws IOException;

    public abstract InputStream getInputStream() throws IOException;

    public abstract OutputStream getOutputStream() throws IOException;

    public abstract Carrier create();


    // things that may be added/changed

    public boolean alternateHeaderCheck(byte[] header) {
	return false; // by default, there is no alternate
    }

    public boolean sendHeader(Protocol proto) throws IOException {
	return proto.defaultSendHeader();
    }

    public void prepareSend(Protocol proto) {
	log.println("prepareSend for " + getName());
    }

    public boolean expectExtraHeader(Protocol proto) throws IOException {
	return true;
    }

    public boolean expectIndex(Protocol proto) throws IOException {
	return proto.defaultExpectIndex();
    }

    public boolean sendAck(Protocol proto) throws IOException {
	return proto.defaultSendAck();
    }

    public boolean respondToHeader(Protocol proto) throws IOException {
	return proto.defaultRespondToHeader();
    }

    public boolean expectSenderSpecifier(Protocol proto) throws IOException {
	return proto.defaultExpectSenderSpecifier();
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
    
    public void close() throws IOException {
	log.println("default Carrier close()");
    }

    public boolean isActive() throws IOException {
	return true;
    }

    public boolean isConnectionless() throws IOException {
	return true;
    }

    public boolean canAccept() {
	return true;
    }

    public boolean canOffer() {
	return true;
    }

    public boolean isTextMode() {
	return false;
    }


    // helper functions

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

    public Address getAddress() throws IOException {
	return getLocalAddress();
    }

    public void open(String carrier, Address address, Carrier prev)
	throws IOException {

	log.error("carrier open not implemented yet");
	System.exit(1);
    }

    // state

    private Address local, remote;

}
