
package yarp.os;

import java.io.*;
import java.net.*;

abstract class AbstractCarrier implements Carrier {

    // things that must be specified

    public abstract String getName();

    public abstract int getSpecifier();

    public abstract void open(Address address, ShiftStream previous) 
	throws IOException;

    public abstract TwoWayStream getStreams();

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

    public boolean sendIndex(Protocol proto) throws IOException {
	return proto.defaultSendIndex();
    }

    public boolean sendAck(Protocol proto) throws IOException {
	return proto.defaultSendAck();
    }

    public abstract boolean respondToHeader(Protocol proto) throws IOException;

    public boolean expectSenderSpecifier(Protocol proto) throws IOException {
	return proto.defaultExpectSenderSpecifier();
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

    public boolean isConnectionless() {
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


    public void start(Address address) throws IOException {
	throw(new IOException("carrier " + getName() + " is not startable"));
    }



    public TwoWayStream takeStreams() {
	log.error("cannot take streams from " + getName());
	System.exit(1);
	return null;
    }



    // helper functions -- these never need to change

    public Address getLocalAddress() throws IOException {
	if (getStreams()==null) return null;
	return getStreams().getLocalAddress();
    }

    public Address getRemoteAddress() throws IOException {
	if (getStreams()==null) return null;
	return getStreams().getRemoteAddress();
    }


    public InputStream getInputStream() throws IOException {
	return getStreams().getInputStream();
    }

    public OutputStream getOutputStream() throws IOException {
	return getStreams().getOutputStream();
    }


    public Address getAddress() throws IOException {
	return getLocalAddress();
    }

    // the logger

    protected static Logger log = Logger.get();
}
