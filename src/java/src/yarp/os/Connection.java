
package yarp.os;

import java.io.*;

// no mention of sockets!  keep it that way...


/**
 * A link between one output-port and one input-port.
 */
class Connection implements ContentWriter {
    private Protocol proto;
    private String fromKey = null;
    private String toKey = null;
    private ContentWriter writer;
    private static Logger log = Logger.get();

    public Connection(Address address, 
		      String carrier,
		      String fromKey, 
		      String toKey,
		      boolean waitForSlow) throws IOException {
	this.fromKey = fromKey;
	this.toKey = toKey;
	
	log.println("*** CONNECTION address " + address + " carrier " + 
		    carrier);

	try {
	    Carrier delegate = Carriers.chooseCarrier(address.getCarrier());
	    delegate.start(address);
	    proto = new Protocol(delegate);
	    
	    //proto.setSender(fromKey);
	    //proto.setReceiver(toKey);
	    proto.setRoute(new Route(fromKey,toKey,
				     proto.getRoute().getCarrierName()));

	    log.println("connection starting address is " + address);
	    proto.setRawProtocol(carrier);
	    proto.sendHeader();
	    proto.expectReplyToHeader();
	    log.println("Connection created ok");
	    if (waitForSlow) {
		writer = new SlowWriter(proto);
	    } else {
		writer = new BackgroundWriter(proto);
	    }
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
	    if (writer!=null) {
		writer.close();
		writer = null;
	    }
	    needExternal = proto.isConnectionless();
	} catch (IOException e) {
	    log.warning("problem closing connection");
	    return;
	}
	log.println("** closing connection");
	proto.close();
	log.println("** connection closed");
	if (needExternal) {
	    // not super-interoperable with C++ YARP
	    log.println("Attempting to ask " + getToName() +
			" to disconnect from " + getFromName());
	    try {
		YarpClient.command(getToName(),"~" +getFromName(),true);
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
	log.assertion(writer!=null);
	writer.write(content,tagAsData);
    }


    public String getCarrierName() throws IOException {
	return proto.getRoute().getCarrierName();
    }
}

