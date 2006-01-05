
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;


class CarrierShiftStream implements ShiftStream {

    Carrier current;

    CarrierShiftStream(Socket socket) {
	try {
	    current = chooseCarrier("tcp");
	    ((TcpCarrier)current).open(socket);
	} catch (IOException e) {
	    System.err.println("failed to open CarrierShiftStream");
	}
    }
    
    private static Carrier[] delegates = { 
	new TcpCarrier(),
	new UdpCarrier(),
	new McastCarrier()
    };

    public static Carrier chooseCarrier(String name) {
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (name.equals(c.getName())) {
		return c;
	    }
	}
	return null;
    }

    public static Carrier chooseCarrier(int specifier) {
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (specifier == c.getSpecifier()) {
		return c;
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
	current.close();
    }

    public void become(String carrier, Address address) throws IOException {
	Carrier next = chooseCarrier(carrier);
	current.close();
	next.open(address,current);
	current = next;
    }

    public Address getAddress() throws IOException {
	return current.getLocalAddress();
    }
}



