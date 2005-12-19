
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.regex.*;

public class NameClient {
    private Address address;

    public NameClient(Address address) {
	this.address = address;
    }

    public static String getProtocolPart(String name) {
	String result = getPart(name,1);
	if (result!=null) {
	    return result;
	}
	return "udp";
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


    public Address mcastQuery(String name) {
	String q = getNamePart(name) + "-mcast";
        Address address = probe("NAME_SERVER query " + q);
	if (address==null) {
	    return register(getNamePart(name),"mcast");
	}
	return address;
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
	return probe("NAME_SERVER register " + name + " 127.0.0.1 " + 
		     rawProtocol);
    }

    public String send(String msg) throws IOException {
	Socket ncSocket = null;
        PrintWriter out = null;
	BufferedReader in = null;

        try {
            ncSocket = new Socket(address.getName(), address.getPort());
            out = new PrintWriter(ncSocket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(ncSocket.getInputStream()));
        } catch (UnknownHostException e) {
            System.err.println("Don't know about host " + address.getName());
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + address.getName());
            System.exit(1);
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


    static NameClient theNameClient = 
	new NameClient(new Address("localhost",10000));
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
