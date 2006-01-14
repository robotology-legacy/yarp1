
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;


class CarrierShiftStream implements ShiftStream {

    Carrier current;
    Carrier incoming;
    private static Logger log = Logger.get();

    CarrierShiftStream(Socket socket) {
	try {
	    current = chooseCarrier("tcp");
	    ((TcpCarrier)current).open(socket);
	} catch (IOException e) {
	    log.error("failed to open CarrierShiftStream");
	}
    }

    public Carrier chooseCarrier(String name) {
	if (current!=null) {
	    if (name.equals(current.getName())) {
		return current;
	    }
	}
	return Carriers.chooseCarrier(name);
    }

    public Carrier chooseCarrier(int specifier) {
	if (current!=null) {
	    if (specifier == current.getSpecifier()) {
		return current;
	    }
	}
	return Carriers.chooseCarrier(specifier);
    }
    
    public Carrier chooseCarrier(byte[] header) {
	if (current!=null) {
	    if (current.alternateHeaderCheck(header)) {
		return current;
	    }
	}
	return Carriers.chooseCarrier(header);
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



