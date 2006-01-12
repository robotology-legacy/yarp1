
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;


class CarrierShiftStream implements ShiftStream {

    Carrier current;
    Carrier incoming;
    Logger log = Logger.get();

    CarrierShiftStream(Socket socket) {
	try {
	    current = chooseCarrier("tcp");
	    ((TcpCarrier)current).open(socket);
	} catch (IOException e) {
	    log.error("failed to open CarrierShiftStream");
	}
    }
    
    private static Carrier[] delegates;

    private static void setup() {
	if (delegates==null) {
	    delegates = new Carrier[] { 
		new TcpCarrier(),
		new UdpCarrier(),
		new McastCarrier()
	    };
	}
    };

    public Carrier chooseCarrier(String name) {
	if (current!=null) {
	    if (name.equals(current.getName())) {
		return current;
	    }
	}
	setup();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (name.equals(c.getName())) {
		return c.create();
		/*
		Carrier prev = current;
		current = c.create();
		if (prev!=null) {
		    current.setAddress(prev.getLocalAddress(),
				       prev.getRemoteAddress());
		}
		return current;
		*/
	    }
	}
	log.error("Could not find carrier " + name + " among "
		  + delegates.length + " carriers");
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    log.println("  " + c.getName());
	}

	return null;
    }

    public Carrier chooseCarrier(int specifier) {
	if (current!=null) {
	    if (specifier == current.getSpecifier()) {
		return current;
	    }
	}
	setup();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (specifier == c.getSpecifier()) {
		return c.create();
		/*
		Carrier prev = current;
		current = c.create();
		if (prev!=null) {
		    current.setAddress(prev.getLocalAddress(),
				       prev.getRemoteAddress());
		}
		return current;
		*/
	    }
	}
	return null;
    }

    public InputStream getInputStream() throws IOException {
	return current.getInputStream();
    }

    public OutputStream getOutputStream() throws IOException {
	return current.getOutputStream();
    }

    public void close() throws IOException {
	log.println("** shift stream closing");
	if (current!=null) {
	    log.println("carrier " + current.getName());
	    current.close();
	} else {
	    log.println("  no carrier");
	}
	log.println("** shift stream closed");
	current = null;
    }

    public void setIncoming(Carrier carrier) {
	incoming = carrier;
    }

    public void become(String carrier, Address address) throws IOException {
	log.println("becoming " + carrier);
	
	Carrier next = current;
	if (!carrier.equals(current.getName())) {
	    log.println("switching from " + current.getName() +
			       " to " + carrier);
	    if (incoming!=null) {
		if (incoming.getName().equals(carrier)) {
		    next = incoming;
		    incoming = null;
		}
	    }
	    if (next==current) {
		next = chooseCarrier(carrier);
	    }
	}
	if (next!=current && !next.getName().equals("tcp")) {
	    current.close();
	    next.open(address,current);
	}
	current = next;
    }

    public Address getAddress() throws IOException {
	return current.getLocalAddress();
    }
}



