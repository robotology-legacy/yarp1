
package yarp.os;

import java.io.*;

abstract class AbstractCarrier implements Carrier {

    // things that must be specified

    public abstract String getName();

    //public abstract int getSpecifier();

    public abstract void open(Address address, ShiftStream previous) 
	throws IOException;

    public abstract TwoWayStream getStreams();

    public abstract Carrier create();

    public abstract byte[] getHeader();


    // things that may be added/changed

    public boolean checkHeader(byte[] header) {
	return false; // by default, there is no alternate
    }

    public void setParameters(byte[] header) {
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

    public boolean requireAck() {
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

    public String toString() {
	return getName();
    }


    protected int getSpecifier(byte[] b) {
	if (b.length==8) {
	    String chk1 = 
		NetType.netString(new byte[] { b[0], b[1], b[6], b[7] });
	    if (chk1.equals("YARP")) {
		int specifier = NetType.unsigned(b[2]) + 
		    NetType.unsigned(b[3])*256;
		specifier -= 7777; // magic number, who knows why
		return specifier;
	    }
	}
	return -1;
    }

   protected byte[] createStandardHeader(int specifier) {
	byte b[] = { 'Y', 'A', 0x64, 0x1e, 0, 0, 'R', 'P' };
	
	int p = specifier;
	p += 7777;

	byte b2[] = NetType.netInt(p);
	for (int i=0; i<b2.length; i++) {
	    b[i+2] = b2[i];
	}
	return b;
    }


    // the logger

    protected static Logger log = Logger.get();
}
