
package yarp.os;

import java.io.*;

// no mention of sockets!  keep it that way...


/**
 * A link between one output-port and one input-port.
 */
class Connection {
    private Address address;
    private Protocol proto;
    private String fromKey = null;
    private String toKey = null;
    private static Logger log = Logger.get();

    public Connection(Address address, 
		      String carrier,
		      String fromKey, 
		      String toKey) throws IOException {
	this.address = address;
	this.fromKey = fromKey;
	this.toKey = toKey;
	
	log.println("*** CONNECTION address " + address + " carrier " + 
		    carrier);

	try {
	    Carrier delegate = Carriers.chooseCarrier(address.getCarrier());
	    delegate.start(address);
	    proto = new Protocol(delegate);
	    
	    proto.setSender(fromKey);
	    log.println("connection starting address is " + address);
	    proto.setRawProtocol(carrier);
	    proto.sendHeader();
	    proto.expectReplyToHeader();
	    log.println("Connection created ok");
	} catch (IOException e) {
	    log.warning("Problem creating connection to " + address);
	}
    }

    public Carrier createCarrier() {
	return new TcpCarrier();
    }

    public String getFromName() {
	return fromKey;
    }

    public String getToName() {
    	return toKey;
    }

    public void close() {
	boolean needExternal = false;
	try {
	    needExternal = proto.isConnectionless();
	} catch (IOException e) {
	    log.warning("problem closing connection");
	}
	log.println("** closing connection");
	proto.close();
	log.println("** connection closed");
	if (needExternal) {
	    // not super-interoperable with C++ YARP
	    log.println("Attempting to ask " + getToName() +
			" to disconnect from " + getFromName());
	    try {
		YarpClient.command(getToName(),"~" +getFromName());
	    } catch (IOException e) {
		// this is fine, other side closed down first
		log.println("request to close other side of connection has problem - maybe already closed");
	    }
	}
    }

    public void write(Content content) throws IOException {
	write(content,true);
    }

    public void write(Content content, boolean tagAsData) throws IOException {
	
	log.assertion(content!=null);

	if (proto.isActive()) {
	    proto.beginContent();
	    if (tagAsData) {
		dataHeader.write(proto);
	    }
	    content.write(proto);
	    proto.endContent();
	    proto.sendIndex();
	    proto.sendContent();
	    proto.expectAck();
	} else {
	    log.println("Skipping explicit output from " + fromKey + 
			" to " + toKey);
	}
    }

    public String getCarrierName() throws IOException {
	return proto.getCarrierName();
    }

    static CommandContent dataHeader = new CommandContent('d',null);

}

