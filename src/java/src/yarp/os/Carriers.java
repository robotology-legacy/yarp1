
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;

public class Carriers {
    private static Carrier[] delegates;
    private static Logger log = Logger.get();

    private static void setup() {
	if (delegates==null) {
	    delegates = new Carrier[] { 
		new TcpCarrier(),
		new UdpCarrier(),
		new McastCarrier(),
		new TextCarrier()
	    };
	}
    };

    public static List collectAccepts() {
	setup();
	List lst = new LinkedList();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (c.canAccept()) {
		lst.add(c.getName());
	    }
	}
	return lst;
    }

    public static List collectOffers() {
	setup();
	List lst = new LinkedList();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (c.canOffer()) {
		lst.add(c.getName());
	    }
	}
	return lst;
    }


    public static Carrier chooseCarrier(String name) {
	setup();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (name.equals(c.getName())) {
		return c.create();
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

    public static Carrier chooseCarrier(int specifier) {
	setup();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (specifier == c.getSpecifier()) {
		return c.create();
	    }
	}
	return null;
    }

    public static Carrier chooseCarrier(byte[] header) {
	setup();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    if (c.alternateHeaderCheck(header)) {
		return c.create();
	    }
	}
	return null;
    }
}

