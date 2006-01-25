
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.*;

/**
 * Client for YARP name server.  There is one global client available
 * from the getNameClient method.
 */
public class NameClient {
    private Address address;
    private String host;
    private static Logger log = Logger.get();

    private NameClient() {
	readConfig();
    }

    /**
     * The address of the name server.
     * @return the address of the name server
     */
    public Address getAddress() { 
	return address;
    }

    private List getIps() {
	List ips = new LinkedList();
	try {
	    Enumeration e = NetworkInterface.getNetworkInterfaces();
	    while (e.hasMoreElements()) {
		NetworkInterface iface = (NetworkInterface)e.nextElement();
		Enumeration e2 = iface.getInetAddresses();
		while (e2.hasMoreElements()) {
		    InetAddress inet = (InetAddress)e2.nextElement();
		    byte[] lst = inet.getAddress();
		    if (lst.length==4) {
			ips.add(inet.getHostAddress());
		    }
		}
	    }
	} catch (Exception e) {
	    // don't worry
	}
	return ips;
    }
    
    String getHostName() {
	if (host==null) {
	    try {
		//InetAddress inet = InetAddress.getLocalHost();
		//host = inet.getCanonicalHostName();

		Enumeration e = NetworkInterface.getNetworkInterfaces();
		while (e.hasMoreElements()) {
		    NetworkInterface iface = (NetworkInterface)e.nextElement();
		    Enumeration e2 = iface.getInetAddresses();
		    while (e2.hasMoreElements()) {
			InetAddress inet = (InetAddress)e2.nextElement();
			byte[] lst = inet.getAddress();
			if (lst.length==4) {
			    log.println("*** inet " + inet + " // " +
					inet.getHostAddress());
			    if (host==null) {
				host = inet.getHostAddress();
			    }
			    if (host.equals("127.0.0.1")) {
				host = inet.getHostAddress();
			    }
			}
		    }
		}
	    } catch (Exception e) {
		host = "127.0.0.1";
	    }
	}

	//log.error("compatibility hack in operation");
	//host = "127.0.0.1";

	log.println("### host name is " + host);
	return host;
    }

    static String getProtocolPart(String name) {
	String result = getPart(name,1);
	if (result!=null) {
	    return result;
	}
	return "tcp";
    }

    private static String getEnv(String key) {
	String val = System.getenv(key);
	if (val!=null) {
	    log.println("Environment variable " + key + " is " + val);
	} else {
	    log.println("Environment variable " + key + " is not set");
	}
	return val;
    }
					 

    private void setConfiguration(String txt) {
	log.println("Configuration text: " + txt);
	Pattern p = Pattern.compile("^([^ ]+) +([0-9]+)");
	Matcher m = p.matcher(txt);
	if (m.find()) {
	    String chost = m.group(1);
	    String cport = m.group(2);
	    log.println("Config host is " + chost + " and port is " +
			cport);
	    address = new Address(chost,Integer.valueOf(cport).intValue());
	}
	//address = new Address("127.0.0.1",10000);
    }


    public void setServerAddress(Address pref) {
	File path = getConfigFile();
	createPath(path);
	writeConfig(path,pref);
	readConfig();
    }


    public File getConfigFile() {
	String root = getEnv("YARP_ROOT");
	String home = getEnv("HOME");
	File conf = null;
	if (root!=null) {
	    conf = new File(new File(root,"conf"),"namer.conf");
	} else if (home!=null) {
	    conf = new File(new File(new File(home,".yarp"),"conf"),"namer.conf");
	} else {
	    log.error("Cannot decide where configuration file is - set YARP_ROOT or HOME");
	    System.exit(1);
	}
	log.println("Configuration file: " + conf);
	return conf;
    }

    private void writeConfig(File path, Address address) {
	File f2 = path;
	if (address==null) {
	    address = new Address(getHostName(),10000);
	}
	if (address.getName()==null) {
	    readConfig();
	    Address prev = getAddress();
	    if (prev!=null) {
		address = new Address(prev.getName(),address.getPort());
	    } else {
		address = new Address(getHostName(),address.getPort());
	    }
	}
	try {
	    PrintStream prn = 
		new PrintStream(new FileOutputStream(f2));
	    prn.println(address.getName() + " " + address.getPort());
	    prn.println("");
	    prn.println("// start network description, don't forget to separate \"Node=\" and names with space");
	    prn.println("[NETWORK_DESCRIPTION]");
	    prn.println("[END]");
	    prn.close();
	} catch (Exception e) {
	    log.error("Problem writing config file: " + e);
	}
    }

    private void createPath(File path) {
	File parent = path.getParentFile();
	if (parent!=null) {
	    if (!parent.exists()) {
		createPath(parent);
		parent.mkdir();
	    }
	}
    }

    private boolean readConfig() {
	File file = getConfigFile();
	if (!file.exists()) {
	    log.warning("Cannot find configuration file");
	    return false;
	}
	StringBuffer txt = new StringBuffer("");
	try {
	    InputStream is = new FileInputStream(file);
	    boolean done = false;
	    while (!done) {
		int v = is.read();
		if (v>=32) {
		    txt.append((char)v);
		} else {
		    done = true;
		}
	    }
	    setConfiguration(txt.toString());
	    is.close();
	} catch (Exception e) {
	    // ok to fail
	    log.warning("Problem reading configuration file");
	    return false;
	}
	return true;
    }

    static String getNamePart(String name) {
	String result = getPart(name,2);
	if (result!=null) {
	    return result;
	}
	return name;
    }

    private static String getPart(String name, int part) {
	Pattern p = Pattern.compile("/?([^/:]+):/([^ ]+)");
	Matcher m = p.matcher(name);
	if (m.find()) {
	    return m.group(part);
	} 
	return null;
    }

    private Address probe(String cmd) {
	String result = "";
	try {
	    result = send(cmd);
	} catch (IOException e) {
	    // don't worry
	}
	return extractAddress(result);
    }

    private Address extractAddress(String txt) {
	Pattern p = Pattern.compile("name ([^ ]+) ip ([^ ]+).*port ([^ ]+).*type ([a-zA-Z]+)");
	Matcher m = p.matcher(txt);
	log.println("Result: " + txt);
	if (m.find()) {
	    String name = m.group(1);
	    String ip = m.group(2);
	    int port = Integer.valueOf(m.group(3)).intValue();
	    String carrier = m.group(4).toLowerCase();

	    // YARP1 compatibility - it lies about the protocol type
	    if (carrier.equals("udp")) { 
		log.error("for YARP1 compatibility, alleged udp ports are reported as tcp");
		carrier = "tcp"; 
	    }

	    if (port!=0) {
		return new Address(ip,port,carrier,name);
	    }
	}
	return null;
    }

    /**
     * Look up the address of a named port.
     * @param name the name of the port
     * @return the address associated with the port
     */
    public Address query(String name) {
	return probe("NAME_SERVER query " + getNamePart(name));
    }

    /**
     * Register a port with a given name.
     * @param name the name of the port
     * @return the address associated with the port
     */
    public Address register(String name) {
	return register(getNamePart(name),getProtocolPart(name));
    }


    /**
     * Register disassociation of name from port.
     * @param name the name to remove
     * @return the new result of queries for that name (should be empty)
     */
    public Address unregister(String name) {
	return probe("NAME_SERVER unregister " + getNamePart(name));
    }

    /**
     * Register a port with a given name and carrier.
     * @param name the name of the port
     * @param name the carrier to use
     * @return the address associated with the port
     */
    public Address register(String name, String rawProtocol) {
	//String host = getHostName();
	Address result = 
	    probe("NAME_SERVER register " + name + " " + rawProtocol);
	name = result.getRegName();

	// set up the protocols that JYARP offers/accepts
	// basically, we just don't do shmem
	probe("NAME_SERVER set " + name + " offers " +
	      NetType.addStrings(Carriers.collectOffers()));
	probe("NAME_SERVER set " + name + " accepts " +
	      NetType.addStrings(Carriers.collectAccepts()));
	probe("NAME_SERVER set " + name + " ips " + NetType.addStrings(getIps()));
	

	return result;
    }

    /**
     * Check if a property of a given port has a given value.
     * @param name the port
     * @param key the property
     * @param val the value
     * @return true if the port's property does have the given value
     */
    public boolean check(String name, String key, String val) {
	boolean ok = true;
	try {
	    String result = send("NAME_SERVER check " + name + " " + key + " " + val);
	    Pattern p = Pattern.compile("port ([^ ]+) property ([^ ]+) value ([^ ]+) present ([^ \n\r]+)");
	    Matcher m = p.matcher(result);
	    log.println("check result is: " + result);
	    if (m.find()) {
		String t = m.group(4);
		if (t.equals("true")) {
		    ok = true;
		} else {
		    ok = false;
		}
	    }
	} catch (IOException e) {
	    log.error("problem with name server");
	}

	return ok;
    }

    /**
     * Set a property of a port to a particular value
     * @param name the port
     * @param key the property
     * @param val the value
     */
    public void set(String name, String key, String val) {
	probe("NAME_SERVER set " + name + " " + key + " " + val);
    }

    /**
     * Set a property of a port to a particular list of values
     * @param name the port
     * @param key the property
     * @param val the values
     */
    public void set(String name, String key, String[] val) {
	set(name,key,NetType.addStrings(val));
    }

    /**
     * Get all the values of a port's property, concatenated into
     * a space-delimited string.
     */
    public String getOne(String name, String key) {
	String result = null;
	String[] str = get(name,key);
	if (str!=null) {
	    result = NetType.addStrings(str);
	}
	return result;
    }

    /**
     * Get all the values of a port's property.
     */
    public String[] get(String name, String key) {
	try {
	    String result = send("NAME_SERVER get " + name + " " + key);
	    Pattern p = Pattern.compile("port ([^ ]+) property ([^ ]+) = ([^\n\r]+)");
	    Matcher m = p.matcher(result);
	    log.println("get result is: " + result);
	    if (m.find()) {
		Pattern p2 = Pattern.compile(" ");
		return p2.split(m.group(4));
	    }
	} catch (IOException e) {
	    log.error("problem with name server");
	}

	return null;
    }


    /**
     * Check if the given port can offer the given protocol
     */
    public boolean offers(String name, String carrier) {
	return check(name,"offers",carrier);
    }

    /**
     * Check if the given port can accept the given protocol
     */
    public boolean accepts(String name, String carrier) {
	return check(name,"accepts",carrier);
    }

    Address seek() {
	final Address addr = FallbackServer.getAddress();
	try {
	    Carrier car = new McastCarrier();
	    car.start(addr);
	    OutputStream out = car.getStreams().getOutputStream();
	    InputStream in = car.getStreams().getInputStream();
	    out.write(NetType.netString("NAME_SERVER query root\n"));
	    out.flush();
	    for (int i=0; i<5; i++) {
		log.info("Broadcast search for name server...");
		if (in.available()>10) {
		    String txt = NetType.readLine(in);
		    log.println("got " + txt);
		    if (txt.length()>=1) {
			if (txt.charAt(0)=='r') {
			    return extractAddress(txt);
			}
		    }
		} else {
		    Time.delay(1);
		}
	    }
	    log.error("No response to broadcast search");
	} catch (IOException e) {
	    log.println("no joy over mcast");
	}
	return null;
    }

    String send(String msg) throws IOException {
	Socket ncSocket = null;
        PrintWriter out = null;
	//BufferedReader in = null;
	boolean done = false;
	int tries = 0;
	String result = "";

	while (!done) {

	    if (address==null) {
		log.info("Configuration file problem; trying multicast fallback");
		address = seek();
		if (address!=null) {
		    setServerAddress(address);
		} else {
		    log.error("Cannot find name server");
		    System.exit(1);
		}
	    }

	    try {
		ncSocket = new Socket(address.getName(), address.getPort());
		out = new PrintWriter(ncSocket.getOutputStream(), true);
		done = true;
		out.println(msg);
		result = NetType.readLines(ncSocket.getInputStream());
		out.close();
		ncSocket.close();
	    } catch (UnknownHostException e) {
		log.error("Don't know about host " + address.getName());
		if (tries>0) {
		    System.exit(1);
		} else {
		    address = null;
		}
	    } catch (IOException e) {
		log.error("Couldn't connect to " + address.getName());
		log.error("Name server missing");
		if (tries>0) {
		    System.exit(1);
		} else {
		    address = null;
		}
	    }
	    tries++;
	}

	return result;
    }


    static NameClient theNameClient = new NameClient();

    /**
     * Get an instance of the name client.
     */
    public static NameClient getNameClient() {
	return theNameClient;
    }

    /**
     * Check if it is possible to connect the given ports with the
     * given protocol.
     */
    public static boolean canConnect(String from, String to, String carrier) {
	NameClient nc = getNameClient();
	boolean canOffer = nc.offers(from,carrier);
	boolean canAccept = nc.accepts(to,carrier);
	Logger.get().println("carrier support: offer " + canOffer + 
			     " accept " + canAccept);
	return canOffer&&canAccept;
    }

}
