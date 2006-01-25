
package yarp.os;

import java.io.*;

/**
 * This class represents the input side of a connection
 */

class InputConnection extends Thread implements ProtocolHandler {
    private static Logger log = Logger.get();
    private InputProtocol proto;
    private Route route;
    private ProtocolHandler handler;

    public InputConnection(InputProtocol proto, 
			   String name,
			   ProtocolHandler handler) throws IOException {
	this.proto = proto;
	this.handler = handler;
	proto.initiate(name);
	route = proto.getRoute();
    }

    public Route getRoute() {
	return route;
    }

    /**
     * return true for eof, false otherwise
     */
    public boolean read(BlockReader reader) throws IOException {
	log.info("Got message, ignoring it completely, and closing connection");
	reader.expectBlock(-1);
	return true;
    }

    public void run() {
	try {

	    boolean done = false;
	    while (!done) {
		BlockReader reader = proto.beginRead();
		done = handler.read(reader);
		proto.endRead();
	    }
	    // give shutdown opportunity
	    handler.read(null);

	} catch (IOException e) {
	    log.println("InputConnection for " + getRoute() + 
			" shutting down: " + e);
	    try {
		proto.close();
		proto = null;
	    } catch (IOException e2) {
		log.println("Further issue for " + getRoute() + ": " + e2);
	    }
	}
    }
}

