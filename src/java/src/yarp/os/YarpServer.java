
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.*;

/*
  implementation of some standard yarp server utilities
 */

class YarpServer implements CommandProcessor {

    private class HostRecord {
	private int base = 10001;
	private Stack reuse = new Stack();

	void release(int port) {
	    reuse.push(new Integer(port));
	}

	int get() {
	    if (reuse.size()>=1) {
		return ((Integer)reuse.pop()).intValue();
	    }
	    int result = base;
	    base++;
	    return result;
	}
    }

    private class NameRecord {
	private Address address = null;

	public void setAddress(Address address) {
	    this.address = address;
	}
	public Address getAddress() {
	    return address;
	}
    }

    private HashMap nameMap = new HashMap();
    private HashMap hostMap = new HashMap();

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
	address = new Address(host,port);
	nameRecord.setAddress(address);
	dump();
	return address;
    }

    private String textify(Address address) {
	String result = "";
	if (address!=null) {
	    result += "ip " + address.getName() + " port " + 
		address.getPort() + " type " + address.getCarrier();
	}
	result += "\n*** end of message\n";
	System.out.println("Response: " + result);
	return result;
    }

    public synchronized String apply(String cmd) {
	dump();
	String response = ">>> command {{" + cmd + "}}\n";

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
		String base = str[1];
		String proto = str[2];
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

