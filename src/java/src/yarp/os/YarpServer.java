
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.*;

/*
  implementation of some standard yarp server utilities
 */

class YarpServer implements CommandProcessor {
    private static Logger log = Logger.get();

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

    private static class DisposableNameRecord extends ReusableRecord {

	// should change this to depend on name server port
	private int base = 1;

	String get() {
	    return "/tmp/port/" + ((Integer)getObject()).intValue();
	}

	Object fresh() {
	    int result = base;
	    base++;
	    return new Integer(result);
	}

	boolean recycle(String name) {
	    Pattern p = Pattern.compile("^/tmp/port/([0-9]+)$");
	    Matcher m = p.matcher(name);
	    if (m.find()) {
		Integer x = Integer.valueOf(m.group(1));
		releaseObject(x);
		return true;
	    }
	    return false;
	}
    }

    private static class HostRecord extends ReusableRecord {

	// should change this to depend on name server port
	private int base = NameClient.getNameClient().getAddress().getPort()+1;

	// YARP2 doesn't need this, just YARP1
	private static final int legacyStep = 10;

	void release(int port) {
	    releaseObject(new Integer(port));
	}

	int get() {
	    return ((Integer)getObject()).intValue();
	}

	Object fresh() {
	    int result = base;
	    base += legacyStep;
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
		log.assertion(base[i]>=1 && base[i]<=255);
	    }
	    return cp;
	}
    }


    private static class PropertyRecord {
	private List prop;

	PropertyRecord() {
	    prop = new LinkedList();
	    clear();
	}

	void clear() {
	    prop.clear();
	}

	boolean check(String p) {
	    return prop.contains(p);
	}

	void add(String p) {
	    prop.add(p);
	}

	public List match(String str) {
	    List lst = new LinkedList();
	    Pattern p = Pattern.compile("^" + str + ".*"); // friends, right?
	    for (Iterator it = prop.iterator(); it.hasNext(); ) {
		String item = (String)it.next();
		Matcher m = p.matcher(item);
		if (m.find()) {
		    lst.add(item);
		}
	    }
	    return lst;
	}

	public String toString() {
	    /*
	    String add = "";
	    StringBuffer out = new StringBuffer("");
	    for (Iterator it = prop.iterator(); it.hasNext(); ) {
		String name = (String)it.next();
		out.append(add);
		out.append(name);
		add = " ";
	    }
	    return out.toString();
	    */
	    return NetType.addStrings(prop);
	}
    }


    private static class NameRecord {
	private Address address = null;
	private Map propMap = new HashMap();

	NameRecord() {
	    // set up some defaults for legacy support
	    addProp("accepts","tcp");
	    addProp("accepts","udp");
	    addProp("accepts","mcast");
	    addProp("accepts","shmem");
	    addProp("offers","tcp");
	    addProp("offers","udp");
	    addProp("offers","mcast");
	    addProp("offers","shmem");
	}

	public PropertyRecord getPropertyRecord(String name, boolean create) {
	    PropertyRecord rec = (PropertyRecord)propMap.get(name);
	    if (create&&rec==null) {
		rec = new PropertyRecord();
		propMap.put(name,rec);
	    }
	    return rec;
	}

	public void setAddress(Address address) {
	    this.address = address;
	}

	public Address getAddress() {
	    return address;
	}

	public void clearProp(String key) {
	    PropertyRecord pr = getPropertyRecord(key,true);
	    pr.clear();
	}

	public void addProp(String key, String val) {
	    PropertyRecord pr = getPropertyRecord(key,true);
	    pr.add(val);
	}

	public String getProp(String key) {
	    PropertyRecord pr = getPropertyRecord(key,false);
	    if (pr!=null) {
		return pr.toString();
	    }
	    return null;
	}

	public boolean checkProp(String key, String val) {
	    PropertyRecord pr = getPropertyRecord(key,false);
	    if (pr!=null) {
		return pr.check(val);
	    }
	    return false;
	}

	public List matchProp(String key, String val) {
	    PropertyRecord pr = getPropertyRecord(key,true);
	    return pr.match(val);
	}
    }

    private HashMap nameMap = new HashMap();
    private HashMap hostMap = new HashMap();
    private McastRecord mcastRecord = new McastRecord();
    private DisposableNameRecord tmpNames = new DisposableNameRecord();

    private static Map getNameParts(String name) {
	Pattern p = Pattern.compile("(/net=([^/]+))?(.*)");
	Matcher m = p.matcher(name);
	Map result = new HashMap();
	result.put("name",name);
	result.put("base",name);
	if (m.find()) {
	    result.put("base",m.group(3));
	    String net = m.group(2);
	    if (net!=null) {
		if (!(net.equals(""))) {
		    result.put("net",m.group(2));
		}
	    }
	} 
	return result;
    }

    public NameRecord getNameRecord(String name) {
	return getNameRecord(name,true);
    }

    public NameRecord getNameRecord(String name, boolean create) {
	Map parts = getNameParts(name);
	return getNameRecord(parts,create);
    }

    public NameRecord getNameRecord(Map parts, boolean create) {
	String base = (String)parts.get("base");
	NameRecord rec = (NameRecord)nameMap.get(base);
	if (rec==null && create) {
	    rec = new NameRecord();
	    nameMap.put(base,rec);
	}
	return rec;
    }

    public void removeNameRecord(String name) {
	nameMap.remove(name);
    }

    public HostRecord getHostRecord(String host) {
	HostRecord rec = (HostRecord)hostMap.get(host);
	if (rec==null) {
	    rec = new HostRecord();
	    hostMap.put(host,rec);
	}
	return rec;
    }

    public String dump() {
	StringBuffer buf = new StringBuffer("");
	for (Iterator it = nameMap.keySet().iterator(); it.hasNext(); ) {
	     String name = (String)it.next();
	     NameRecord rec = getNameRecord(name);
	     buf.append(textify(rec.getAddress()));
	}
	return buf.toString();
    }

    public Address unregister(String name) {
	NameRecord rec = getNameRecord(name,false);
	if (rec!=null) {
	    Address address = rec.getAddress();
	    recycle(address);
	}
	removeNameRecord(name);
	tmpNames.recycle(name);
	return query(name);
    }

    public Address query(String name) {
	Map parts = getNameParts(name);
	NameRecord rec = getNameRecord(parts,false);
	if (rec!=null) {
	    Address address = rec.getAddress();
	    String net = (String)parts.get("net");
	    if (net!=null) {
		log.println("special net request: " + net);
		List ips = rec.matchProp("ips",net);
		if (ips.size()==0) {
		    NameRecord networks = getNameRecord("networks",false);
		    if (networks!=null) {
			String alias = networks.getProp(net);
			if (alias!=null) {
			    ips = rec.matchProp("ips",alias);
			}
		    }
		}
		if (ips.size()>=1) {
		    address = new Address(ips.get(0).toString(),
					  address.getPort(),
					  address.getCarrier(),
					  address.getRegName());
		}
	    }
	    return address;
	}
	return null;
    }


    public void recycle(Address address) {
	if (address!=null) {
	    log.println("Reusing port " + address.getPort() +
			" on host " + address.getName());
	    getHostRecord(address.getName()).release(address.getPort());
	}
    }

    public Address register(String name,
			    String host,
			    int sock,
			    String protocol,
			    int count) {
	NameRecord nameRecord = getNameRecord(name);
	HostRecord hostRecord = getHostRecord(host);
	Address address = nameRecord.getAddress();
	recycle(address);
	if (count!=1) {
	    log.println("Count is " + count + 
			" but this is currently ignored (legacy)");
	}
	int port = sock;
	if (sock==-1) {
	    port = hostRecord.get();
	}
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
	address = new Address(host,port,protocol,name);
	nameRecord.setAddress(address);
	//dump();
	return address;
    }

    private String terminate(String str) {
	return str + "*** end of message\n";
    }

    private String textify(Address address) {
	String result = "";
	if (address!=null) {
	    result = "registration name " + address.getRegName() + 
		" ip " + address.getName() + " port " + 
		address.getPort() + " type " + address.getCarrier() + "\n";
	}
	return result;
    }

    public synchronized String apply(String cmd, Address address) {
	//dump();
	String response = "Unsupported request [" + cmd + "] from " + address + "\n";

	Pattern p = Pattern.compile("^(NAME_SERVER )([^ \n\r]+)( ([^\n\r]*))?");
	Matcher m = p.matcher(cmd);
	if (m.find()) {
	    log.info("command -- " + cmd);
	    String act = m.group(2);
	    log.println("act is [" + act + "]");
	    Pattern p2 = Pattern.compile(" ");
	    String strs = m.group(4);
	    if (strs==null) { strs = ""; }
	    String[] str = p2.split(strs);
	    if (act.equals("register")) {
		String target = str[0];
		String base = null;
		String sock = null;
		String proto = null;
		String count = null;
		if (target.equals("...")) {
		    target = tmpNames.get();
		}
		if (str.length>=2) {
		    proto = str[1];
		    if (proto.equals("...")) {
			proto = null;
		    }
		    if (str.length>=3) {
			base = str[2];
			if (base.equals("...")) {
			    base = null;
			}
			if (str.length>=4) {
			    sock = str[3];
			    if (sock.equals("...")) {
				sock = null;
			    }
			    if (str.length>=5) {
				count = str[4];
				if (count.equals("...")) {
				    count = null;
				}
			    }
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
		if (count==null) {
		    count = "1";
		}
		int nsock = -1;
		int ct = 1;
		try {
		    if (sock!=null) {
			nsock = Integer.valueOf(sock).intValue();
		    }
		    //log.info("registered " + target + " for " +
		    //base + " with protocol " + proto);
		    ct = Integer.valueOf(count).intValue();
		} catch (NumberFormatException e) {
		    log.error("bad number format: check " + sock + " and " + count);
		}
		Address result = register(target,base,nsock,proto,ct);
		response = terminate(textify(result));
	    }
	    if (act.equals("unregister")) {
		String target = str[0];
		log.info("Unregister " + target);
		Address result = unregister(target);
		response = terminate(textify(result));
	    }
	    if (act.equals("query")) {
		String target = str[0];
		log.info("Query " + target);
		Address result = query(target);
		response = terminate(textify(result));
	    }
	    if (act.equals("list")) {
		response = terminate(dump());
	    }
	    if (act.equals("set")) {
		String target = str[0];
		String key = str[1];
		NameRecord nameRecord = getNameRecord(target);
		nameRecord.clearProp(key);
		for (int i=2; i<str.length; i++) {
		    nameRecord.addProp(key,str[i]);
		}
		response = "port " + target + " property " + 
		    key + " = " + nameRecord.getProp(key) +
		    "\n";
	    }
	    if (act.equals("get")) {
		String target = str[0];
		String key = str[1];
		NameRecord nameRecord = getNameRecord(target);
		response = "port " + target + " property " + 
		    key + " = " + nameRecord.getProp(key) +
		    "\n";
	    }
	    if (act.equals("match")) {
		String target = str[0];
		String key = str[1];
		String val = str[2];
		NameRecord nameRecord = getNameRecord(target);
		response = "port " + target + " property " + 
		    key + " = " + NetType.addStrings(nameRecord.matchProp(key,val)) +
		    "\n";
	    }
	    if (act.equals("check")) {
		String target = str[0];
		String key = str[1];
		NameRecord nameRecord = getNameRecord(target);
		response = "";
		for (int i=2; i<str.length; i++) {
		    String val = "false";
		    if (nameRecord.checkProp(key,str[i])) {
			val = "true";
		    }
		    response += "port " + target + " property " + 
			key + " value " + str[i] + " present " + val +
			"\n";
		}
	    }
	    if (act.equals("connect")) {
		log.println("Long-term connection mode");
		response = null;
	    }
	    if (act.equals("quit")) {
		log.println("Long-term connection mode ended");
		response = null;
	    }
	} else {
	    log.error("Unsupported message received");
	    boolean suspect = false;
	    for (int i=0; i<cmd.length(); i++ ) {
		if (cmd.charAt(i)>=128) {
		    suspect = true;
		}
	    }
	    if (cmd.length()>=1) {
		if (cmd.charAt(0)=='@') {
		    suspect = true;
		}
	    }
	    if (suspect) {
		log.println("Looks like YARP1 connection");
		response = "???????????????????????????????????????????";
	    }
	}
	if (response!=null) {
	    log.info(response);
	}
	return response;
    }

    public static void run(boolean background, Address pref) {

	NameClient nc = NameClient.getNameClient();

	Address address = nc.getAddress();

	if (pref!=null||address==null) {
	    nc.setServerAddress(pref);
	}

	address = nc.getAddress();

	try {
	    InetAddress inet = InetAddress.getByName(address.getName());
	    if (!(inet.isLoopbackAddress()||
		  inet.isAnyLocalAddress()||
		  address.getName().equals(nc.getHostName()))) {
		// don't know what this test actually does; don't have
		// any documentation
		log.error("Name server is configured for a remote address " + 
			  address);
		System.exit(1);
	    }
	    YarpServer server = new YarpServer();
	    TelnetPort tp = new TelnetPort(address,server);
	    log.info("name server starting at " + address);
	    if (background) {
		tp.start();
	    } else {
		tp.run();
	    }
	} catch (Exception e) {
	    log.error("Can't create name server: " + e);
	}
    }

    public static void main(String[] args) {
	if (args.length>=2) {
	    if (args.length>=3) {
		run(false,new Address(args[1],
				      Integer.valueOf(args[2]).intValue()));
	    } else {
		run(false,new Address(null,
				      Integer.valueOf(args[1]).intValue()));
	    }
	} else {
	    run(false,null);
	}
    }
}

