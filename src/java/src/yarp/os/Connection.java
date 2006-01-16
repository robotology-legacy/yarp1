
package yarp.os;

import java.io.*;
import java.net.*;

/**
 * A link between one output-port and one input-port.
 */
class Connection {
    private Address address;
    private Protocol proto;
    private Socket socket = null;
    private String fromKey = null;
    private String toKey = null;
    private static Logger log = Logger.get();

    public Connection(Address address, 
		      String fromKey, 
		      String toKey) throws IOException {
	this.address = address;
	this.fromKey = fromKey;
	this.toKey = toKey;

	try {
	    socket = new Socket(address.getName(),address.getPort());
	} catch (UnknownHostException e) {
	    throw(new IOException("Don't know about host " + address.getName()));
	} catch (IOException e) {
	    //log.error("Couldn't make a connection to " + address);
	    throw(new IOException("Couldn't make a connection to " + address));
	}
	    
	if (socket==null) {
	    System.exit(1);
	}
	    
	try {
	    
	    proto = new Protocol(new TcpCarrier(socket));
	    //proto = new Protocol(new CarrierShiftStream(socket));
	    
	    proto.setSender(fromKey);
	    log.println("connection starting address is " + address);
	    proto.setRawProtocol(address.getCarrier());
	    proto.sendHeader();
	    proto.expectReplyToHeader();
	    log.println("Connection created ok");
	} catch (IOException e) {
	    log.warning("Problem creating connection to " + address);
	}
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
	    // probably not interoperable with C++ YARP, need to look
	    // up what the right command is
	    log.println("Attempting to ask " + getToName() +
			" to disconnect from " + getFromName());
	    try {
		YarpClient.command(getToName(),"~" +getFromName());
	    } catch (IOException e) {
		// this is fine, other side closed down first
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
		// data header
		//proto.addContent(new byte[] { 0,0,0,0,  '~', 'd', 0, 1}); 
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

