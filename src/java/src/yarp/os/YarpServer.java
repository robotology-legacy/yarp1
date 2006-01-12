
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.*;

/*
  implementation of some standard yarp server utilities
 */

class YarpServer implements CommandProcessor {

    private static abstract class ReusableRecord {

	private Stack reuse = new Stack();

	abstract Object fresh();

	void releaseObject(Object o) {
	    reuse.push(o);
	}

	Object getObject() {
	    if (reuse.size()>=1) {
		return reuse.pop();
	    }
	    return fresh();
	}
    }

    private static class HostRecord extends ReusableRecord {

	// should change this to depend on name server port
	private int base = 10001;

	void release(int port) {
	    releaseObject(new Integer(port));
	}

	int get() {
	    return ((Integer)getObject()).intValue();
	}

	Object fresh() {
	    int result = base;
	    base++;
	    return new Integer(result);
	}
    }

    private static class McastRecord extends ReusableRecord {

	private int base[] = { 224,1,1,1 };

	void release(int[] ip) {
	    releaseObject(ip);
	}

	int[] get() {
	    return (int[])getObject();
	}

	Object fresh() {
	    int cp[] = new int[4];
	    for (int i=0; i<cp.length; i++) {
		cp[i] = base[i];
	    }
	    base[3]++;
	    if (base[3]>255) {
		base[2]++;
		base[3] = 1;
	    }
	    for (int i=0; i<base.length; i++) {
		assert(base[i]>=1 && base[i]<=255);
	    }
	    return cp;
	}
    }


    private static class ProtocolRecord {
	private Set spoken;

	ProtocolRecord() {
	    spoken = new HashSet();
	    clear();
	}

	void clear() {
	    spoken.clear();
	    spoken.add(getLinguaFranca());
	}

	static String getLinguaFranca() {
	    return "tcp";
	}

	boolean check(String p) {
	    return spoken.contains(p);
	}

	void add(String p) {
	    spoken.add(p);
	}
    }


    private static class NameRecord {
	private Address address = null;
        private ProtocolRecord accepts = new ProtocolRecord();
	private ProtocolRecord offers = new ProtocolRecord();

	public void setAddress(Address address) {
	    this.address = address;
	}
	public Address getAddress() {
	    return address;
	}
	ProtocolRecord getAccepts() {
	    return accepts;
	}
	ProtocolRecord getOffers() {
	    return offers;
	}
	/*
	boolean checkAccepts(String p) {
	    return accepts.check(p);
	}
	boolean checkOffers(String p) {
	    return offers.check(p);
	}
	void addAccept(String p) {
	    accepts.add(p);
	}
	void addOffer(String p) {
	    offers.add(p);
	}
	*/
    }

    private HashMap nameMap = new HashMap();
    private HashMap hostMap = new HashMap();
    private McastRecord mcastRecord = new McastRecord();

    public NameRecord getNameRecord(String name) {
	NameRecord rec = (NameRecord)nameMap.get(name);
	if (rec==null) {
	    rec = new NameRecord();
	    nameMap.put(name,rec);
	}
	return rec;
    }

    public HostRecord getHostRecord(String host) {
	HostRecord rec = (HostRecord)hostMap.get(host);
	if (rec==null) {
	    rec = new HostRecord();
	    hostMap.put(host,rec);
	}
	return rec;
    }

    public void dump() {
	PrintStream out = System.out;
	out.println("Name records");
	for (Iterator it = nameMap.keySet().iterator(); it.hasNext(); ) {
	     String name = (String)it.next();
	     NameRecord rec = getNameRecord(name);
	     out.println("  Key [" + name + "] has address " + rec.getAddress());
	}
    }


    public Address query(String name) {
	NameRecord rec = (NameRecord)nameMap.get(name);
	if (rec!=null) {
	    return rec.getAddress();
	}
	return null;
    }


    public Address register(String name,
			    String host,
			    String protocol) {
	NameRecord nameRecord = getNameRecord(name);
	HostRecord hostRecord = getHostRecord(host);
	Address address = nameRecord.getAddress();
	if (address!=null) {
	    System.out.println("Reusing port " + address.getPort() +
			       " on host " + address.getName());
	    getHostRecord(address.getName()).release(address.getPort());
	}
	int port = hostRecord.get();
	if (protocol.equals("mcast")) {
	    int ip[] = mcastRecord.get();
	    Address mcastAddress = new Address("" + ip[0] + "." + 
					       ip[1] + "." +
					       ip[2] + "." +
					       ip[3],
					       port,
					       protocol);
	    
	    host = mcastAddress.getName();
	}
	address = new Address(host,port,protocol);
	nameRecord.setAddress(address);
	dump();
	return address;
    }

    private String textify(Address address) {
	String result = "";
	if (address!=null) {
	    result += "registration ip " + address.getName() + " port " + 
		address.getPort() + " type " + address.getCarrier();
	}
	result += "\n*** end of message\n";
	System.out.println("Response: " + result);
	return result;
    }

    public synchronized String apply(String cmd, Address address) {
	dump();
	String response = ">>> command [" + cmd + "] from " + address + "\n";

	Pattern p = Pattern.compile("^NAME_SERVER ([^ ]+) ([^\n\r]*)");
	Matcher m = p.matcher(cmd);
	if (m.find()) {
	    System.out.println(">>> " + cmd);
	    String act = m.group(1);
	    System.out.println("act is " + act);
	    Pattern p2 = Pattern.compile(" ");
	    String[] str = p2.split(m.group(2));
	    if (act.equals("register")) {
		String target = str[0];
		String base = null;
		String proto = null;
		if (str.length>=2) {
		    proto = str[1];
		    if (proto.equals("*")) {
			proto = null;
		    }
		    if (str.length>=3) {
			base = str[2];
			if (base.equals("*")) {
			    base = null;
			}
		    }
		} 
		if (proto==null) {
		    proto = "tcp";
		}
		if (base==null) {
		    base = address.getName();
		    if (base.equals("127.0.0.1")) {
			base = NameClient.getNameClient().getHostName();
		    }
		}
		System.out.println("Register " + target + " for " +
				   base + " with protocol " + proto);
		Address result = register(target,base,proto);
		return textify(result);
	    }
	    if (act.equals("query")) {
		String target = str[0];
		System.out.println("Query " + target);
		Address result = query(target);
		return textify(result);
	    }
	    if (act.equals("offer")) {
		String target = str[0];
		System.out.println("Offer " + target);
		NameRecord nameRecord = getNameRecord(target);
		ProtocolRecord pr = nameRecord.getOffers();
		pr.clear();
		for (int i=1; i<str.length; i++) {
		    pr.add(str[i]);
		}
	    }
	} else {
	    System.err.println("Unsupported message received\n");
	}
	return response;
    }

    public static void run(boolean background) {
	YarpServer server = new YarpServer();

	System.out.println("Working on a name-server implementation");
	Address address = NameClient.getNameClient().getAddress();
	try {
	    InetAddress inet = InetAddress.getByName(address.getName());
	    if (inet.isLoopbackAddress()||inet.isAnyLocalAddress()) {
		// don't know what this test actually does; don't have
		// any documentation
		TelnetPort tp = new TelnetPort(address,server);
		if (background) {
		    tp.start();
		} else {
		    tp.run();
		}
	    } else {
		System.out.println("Name server is configured for a remote address");
	    }
	} catch (Exception e) {
	    System.out.println("Can't look up host");
	}
    }

    public static void main(String[] args) {
	run(false);
    }
}

