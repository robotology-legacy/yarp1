
package yarp.os;

import java.io.*;
import java.util.*;


/*
  implementation of some standard yarp client utilities
 */

class YarpClient {

    public static void read(String[] args) {
	if (args.length<2) {
	    System.err.println("use as: <this program> read /name");
	    System.exit(1);
	}
	String name = args[1];

	InputPort in = new InputPort();
	in.creator(new BottleContent());
	in.register(name);
	boolean done = false;
	while (!done) {
	    if (in.read()) {
		Bottle bot = (Bottle)in.content();
		System.out.println("Got " + bot);
		List lst = bot.asList();
		if (lst.size()>=1) {
		    Object o = lst.get(0);
		    if (o instanceof Integer) {
			if (((Integer) o).intValue()==1) {
			    done = true;
			    // honor end-of-stream marker
			}
		    }
		}
	    }
	}
	in.close();
    }


    public static String get() {
	InputStream in = System.in;
	StringBuffer buf = new StringBuffer("");
	try {
	    while (true) {
		System.out.flush();
		int r = in.read();
		if (r<0) {
		    throw new IOException();
		}
		char ch = (char)r;
		if (ch!='\n'&&ch!='\r') {
		    buf.append(ch);
		}
		if (ch=='\n') {
		    return buf.toString();
		}
	    }
	} catch (IOException e) {
	    return null;
	}
    }


    public static void command(String source, String cmd) {
	try {
	    NameClient nc = NameClient.getNameClient();
	    
	    Address add = nc.query(source);
	    
	    if (add==null) {
		System.err.println("Cannot find port " + source);
		return;
	    }
	    
	    Connection c = new Connection(add,"external",source);
	    c.writeCommand('\0',cmd);
	} catch (IOException e) {
	    System.err.println("connection failed");
	}
    }

    public static void connect(String[] args) {
	if (args.length<3) {
	    System.err.println("use as: <this program> connect /src /target");
	    System.exit(1);
	}
	String source = args[1];
	String target = args[2];
	System.out.println("Connecting " + source + " to " + target);
	command(source,target);
    }

    public static void disconnect(String[] args) {
	if (args.length<3) {
	    System.err.println("use as: <this program> disconnect /src /target");
	    System.exit(1);
	}
	String source = args[1];
	String target = args[2];
	System.out.println("Disconnecting " + source + " from " + target);
	command(source,"!"+target);
    }

    public static void write(String[] args) {
	if (args.length<2) {
	    System.err.println("use as: <this program> write /name [/target]");
	    System.exit(1);
	}
	String name = args[1];

	OutputPort out = new OutputPort();
	out.creator(new BottleContent());
	out.register(name);
	for (int i=2; i<args.length; i++) {
	    out.connect(args[i]);
	}
	
	boolean done = false;
	while (!done) {
	    String str = get();
	    boolean eof = false;
	    if (str==null) {
		eof = true;
		str = "<EOF>";
		done = true;
	    }
	    Bottle bot = (Bottle) out.content();
	    bot.clear();
	    bot.add(new Integer(eof?1:0));
	    bot.add(str);
	    out.write();
	}
	Time.delay(1);

	out.close();
    }


    public static void main(String[] args) {
	if (args.length>0) {
	    System.out.println("first arg is " + args[0]);
	    String mode = args[0];
	    if (mode.equals("read")) {
		read(args);
	    }
	    if (mode.equals("write")) {
		write(args);
	    }
	    if (mode.equals("connect")) {
		connect(args);
	    }
	    if (mode.equals("disconnect")) {
		disconnect(args);
	    }
	    if (mode.equals("server")) {
		YarpServer.main(args);
	    }
	} else {
	    System.err.println("here are ways to use this program:");
	    System.err.println("  <this program> read /name");
	    System.err.println("  <this program> write /name [/target]");
	    System.err.println("  <this program> connect /source /target");
	    System.err.println("  <this program> disconnect /source /target");
	    System.err.println("  <this program> server");
	}
    }

}

