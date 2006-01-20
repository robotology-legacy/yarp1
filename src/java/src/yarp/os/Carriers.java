
package yarp.os;

import java.util.*;

class Carriers {
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

    private static Carrier chooseCarrier(String name, byte[] header) {
	setup();
	for (int i=0; i<delegates.length; i++) {
	    Carrier c = delegates[i];
	    boolean match = false;
	    if (name!=null) {
		if (name.equals(c.getName())) {
		    match = true;
		}
	    }
	    if (header!=null) {
		if (c.checkHeader(header)) {
		    return c.create();
		}
	    }
	    if (match) {
		return c.create();
	    }
	}
	log.error("Could not find carrier");
	return null;
    }

    public static Carrier chooseCarrier(String name) {
	return chooseCarrier(name,null);
    }

    public static Carrier chooseCarrier(byte[] header) {
	return chooseCarrier(null,header);
    }
}

