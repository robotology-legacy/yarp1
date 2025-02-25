
package yarp.os;

import java.io.*;

class TextCarrier extends TcpCarrier {
    //private static byte[] altHeader = NetType.netString("CONNECT ");

    public String getName() {
	return "text";
    }

    public String getSpecifierName() {
	return "CONNECT ";
    }

    public boolean checkHeader(byte[] header) {
	if (header.length==8) {
	    if (header[0]=='C') {
		if (NetType.netString(header).equals(getSpecifierName())) {
		    return true;
		}
	    }
	}
	return false;
    }

    public byte[] getHeader() {
	return NetType.netString(getSpecifierName());
    }


    public Carrier create() {
	log.println("*** TextCarrier::create()");
	return new TextCarrier();
    }

    public boolean canOffer() {
	// can't output in this format yet
	//return false;

	// ok let's try
	return true;
    }

    public boolean sendHeader(Protocol proto) throws IOException {
	proto.write(NetType.netString("CONNECT " + 
				      proto.getRoute().getFromName() + "\n"));
	setRequireAck(false);
	return true;
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	log.println("expectReplyToHeader for text");
	proto.become(this,getLocalAddress());
    }

    public boolean expectSenderSpecifier(Protocol proto) throws IOException {
	proto.setRoute(proto.getRoute().addFromName(proto.readLine()));
	return true;
    }

    public boolean sendIndex(Protocol proto) throws IOException {
	log.println("Text mode - skipping index stage");
	return true;
    }

    public boolean expectIndex(Protocol proto) throws IOException {
	log.println("Text mode - skipping index stage");
	return true;
    }

    public boolean sendAck(Protocol proto) throws IOException {
	log.println("Text mode - skipping send ack");
	return true;
    }


    public boolean respondToHeader(Protocol proto) throws IOException {
	byte[] b = NetType.netString("Welcome " + 
				     proto.getRoute().getFromName() + "\n");
	proto.write(b);
	proto.become(this,getLocalAddress());
	return true;
    }

    public boolean isTextMode() {
	return true;
    }

    public void open(Address address, ShiftStream previous) 
	throws IOException {
	way = previous.takeStreams();
    }
}

