
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.*;

public class NameClient {
    private Address address;
    private String host;

    public NameClient(Address address) {
	this.address = address;
    }

    public NameClient() {
	getConfiguration();
    }

    public Address getAddress() { 
	return address;
    }
    
    public String getHostName() {
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
			    System.err.println("*** inet " + inet + " // " +
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
	System.err.println("### host name is " + host);
	return host;
    }

    public static String getProtocolPart(String name) {
	String result = getPart(name,1);
	if (result!=null) {
	    return result;
	}
	return "udp";
    }

    public static String getConfiguration(String key) {
	String val = System.getenv(key);
	if (val!=null) {
	    System.err.println("Environment variable " + key + " is " + val);
	} else {
	    System.err.println("Environment variable " + key + " is not set");
	}
	return val;
    }
					 

    public void setConfiguration(String txt) {
	System.err.println("Configuration text: " + txt);
	Pattern p = Pattern.compile("^([^ ]+) +([0-9]+)");
	Matcher m = p.matcher(txt);
	if (m.find()) {
	    String host = m.group(1);
	    String port = m.group(2);
	    System.err.println("Config host is " + host + " and port is " +
			       port);
	    address = new Address(host,Integer.valueOf(port));
	}
	address = new Address("127.0.0.1",10000);
    }

    public void getConfiguration() {
	String root = getConfiguration("YARP_ROOT");
	String home = getConfiguration("HOME");
	String conf = null;
	if (root!=null) {
	    conf = root + "/conf/namer.conf";
	} else if (home!=null) {
	    conf = home + "/.yarp/conf/namer.conf";
	} else {
	    System.err.println("Cannot decide where configuration file is - set YARP_ROOT or HOME");
	    System.exit(1);
	}
	System.err.println("Configuration file: " + conf);
	File file = new File(conf);
	if (!file.exists()) {
	    System.err.println("Cannot find configuration file");
	    String base = root;
	    if (base==null) {
		base = home + "/.yarp";
		File f1 = new File(base);
		if (!f1.exists()) {
		    f1.mkdir();
		}
	    }
	    if (base!=null) {
		File f1 = new File(base + "/conf");
		if (!f1.exists()) {
		    f1.mkdir();
		}
		File f2 = new File(base + "/conf/namer.conf");
		if (!f2.exists()) {
		    try {
			PrintStream prn = new PrintStream(f2);
			prn.println("127.0.0.1 10000");
			prn.println("");
			prn.println("// start network description, don't forget to separate \"Node=\" and names with space");
			prn.println("[NETWORK_DESCRIPTION]");
			prn.println("[END]");
			conf = f2.getName();
			file = f2;
			prn.close();
		    } catch (Exception e) {
			// ok to fail
		    }
		}
	    }
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
	    System.err.println("Problem reading configuration file");
	}
    }

    public static String getNamePart(String name) {
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

    public Address probe(String cmd) {
	String result = "";
	try {
	    result = send(cmd);
	} catch (IOException e) {
	}
	Pattern p = Pattern.compile("ip ([^ ]+).*port ([^ ]+).*type ([^ ]+)");
	Matcher m = p.matcher(result);
	System.out.println("Result: " + result);
	if (m.find()) {
	    String ip = m.group(1);
	    int port = Integer.valueOf(m.group(2)).intValue();
	    String carrier = m.group(3).toLowerCase();
	    if (port!=0) {
		return new Address(ip,port,carrier);
	    }
	}
	return null;
    }

    public Address normalQuery(String name) {
	return probe("NAME_SERVER query " + getNamePart(name));
    }

    public Address query(String name) {
	return probe("NAME_SERVER query " + getNamePart(name));
    }

    public Address register(String name) {
	return register(getNamePart(name),getProtocolPart(name));
    }

    public Address register(String name, String rawProtocol) {
	String host = getHostName();
	return probe("NAME_SERVER register " + name + " 127.0.0.1 " + 
		     rawProtocol);
    }

    public String send(String msg) throws IOException {
	Socket ncSocket = null;
        PrintWriter out = null;
	BufferedReader in = null;
	int tries = 0;
	boolean done = false;

	while (tries<3&&!done) {
	    try {
		tries++;
		ncSocket = new Socket(address.getName(), address.getPort());
		out = new PrintWriter(ncSocket.getOutputStream(), true);
		in = new BufferedReader(new InputStreamReader(ncSocket.getInputStream()));
		done = true;
	    } catch (UnknownHostException e) {
		System.err.println("Don't know about host " + address.getName());
		System.exit(1);
	    } catch (IOException e) {
		System.err.println("Name server missing");
		System.err.println("Couldn't get I/O for the connection to " + address.getName());
		//System.exit(1);
		System.err.println("Starting a LOCAL server, just so something will work");
		System.err.println("You should really make a name server yourself");
		getConfiguration();
		Time.delay(3);
		YarpServer.run(true);
		Time.delay(1);
	    }
	}


	out.println(msg);

	StringBuffer result = new StringBuffer();
        String fromServer;
	boolean first = true;
        while ((fromServer = in.readLine()) != null) {
	    if (fromServer.length()>0) {
		if (fromServer.charAt(0)>=32) {
		    if (!first) {
			result.append("\n");
		    }
		    result.append(fromServer);
		}
	    }
	    first = false;
        }

        out.close();
        in.close();
        ncSocket.close();

	return result.toString();
    }


    static NameClient theNameClient = new NameClient();

    //new NameClient(new Address("localhost",10000));


    public static NameClient getNameClient() {
	return theNameClient;
    }

    public static void main(String[] args) throws IOException {
	
	NameClient nc = new NameClient(new Address("localhost",10000));
	System.out.println(nc.send("NAME_SERVER query /frank"));
	System.out.println(nc.send("NAME_SERVER register /frank 44.44.44.44"));
	System.out.println(nc.send("NAME_SERVER query /frank"));

	//nc.send2("bozo");
    }
}
