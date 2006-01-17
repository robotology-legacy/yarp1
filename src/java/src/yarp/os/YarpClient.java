
package yarp.os;

import java.io.*;
import java.util.*;


/**
 * Implementation of a standard set of YARP utilities.
 */

public class YarpClient {
    private static Logger log = Logger.get();

    private static String[] subArray(String[] str, int start, int len) {
	String[] out = new String[len];
	System.arraycopy(str,start,out,0,len);
	return out;
    }

    private static void remote(String[] args) {
	if (args.length>=2) {
	    String target = args[1];
	    String portStr = "10000";
	    if (args.length>=3) {
		portStr = args[2];
	    }
	    int port = Integer.valueOf(portStr).intValue();
	    Address address = new Address(target,port);
	    NameClient.getNameClient().setServerAddress(address);
	    System.out.println("*** set name server address to " + address);
	}
	where();
    }

    /**
     * Create a port to read Bottles and prints them to standard input.
     * It assumes the Bottles consist of an integer followed by a string.
     * The integer indicates whether the "end-of-file" has been reached.
     * The string is what gets printed.
     * @param name the name which which to register the port
     */
    public static void read(String name) {

	InputPort in = new InputPort();
	in.creator(new BottleContent());
	in.register(name);
	boolean done = false;
	while (!done) {
	    if (in.read()) {
		Bottle bot = (Bottle)in.content();
		List lst = bot.asList();
		boolean shown = false;
		if (lst.size()>=2) {
		    Object o = lst.get(0);
		    if (o instanceof Integer) {
			if (((Integer) o).intValue()==1) {
			    done = true;
			    shown = true;
			    log.println("should honor end-of-stream marker");
			}
		    }
		    if (!shown) {
			Object o1 = lst.get(1);
			System.out.println(o1);
			shown = true;
		    }
		}
		if (!shown) {
		    System.out.println("Got " + bot);
		}
	    }
	}
	log.println("ending YarpClient.read");
	in.close();
	log.println("ended YarpClient.read");
    }


    private static void read(String[] args) {
	if (args.length<2) {
	    System.err.println("use as: <this program> read /name");
	    System.exit(1);
	}
	String name = args[1];
	read(name);
    }



    private static String get() {
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

    static void command(String source, String cmd) throws IOException {
	command(source,cmd,'\0');
    }

    static void command(String source, String cmd, char ch) throws IOException {
	try {
	    NameClient nc = NameClient.getNameClient();
	    
	    Address add = nc.query(source);
	    
	    if (add==null) {
		System.err.println("Cannot find port " + source);
		return;
	    }
	    
	    // to be nice to newcomers, let's allows commands in alt carriers
	    // (i.e. text)
	    add = new Address(add.getName(), add.getPort(), 
			      NameClient.getProtocolPart(source));
	    
	    Connection c = new Connection(add,"external",source);
	    log.println("command is [" + cmd + "] and char is [" + ((int)ch) + "]");
	    c.write(new CommandContent(ch,cmd), false);
	    //c.writeCommand(ch,cmd);
	    c.close();
	} catch (IOException e) {
	    throw(new IOException("connection failed"));
	}
    }

    private static void check() {
	InputPort in = new InputPort();
	OutputPort out = new OutputPort();
	IntegerContent ints = new IntegerContent();
	System.out.println("==================================================================");
	System.out.println("=== Trying to register some ports");
	in.creator(ints);
	out.creator(ints);
	in.register("...");
	out.register("...");
	Time.delay(1);
	System.out.println("==================================================================");
	System.out.println("=== Trying to connect some ports");
	out.connect(in.name());
	Time.delay(1);
	System.out.println("==================================================================");
	System.out.println("=== Trying to write some data");
	out.write(new IntegerContent(42));
	Time.delay(1);
	System.out.println("==================================================================");
	boolean ok = false;
	for (int i=0; i<3; i++) {
	    System.out.println("=== Trying to read some data");
	    Time.delay(1);
	    if (in.read(true)) {
		int x = ((Integer)in.content()).intValue();
		System.out.println("*** Read number " + x);
		if (x==42) {
		    ok = true;
		    break;
		}
	    }
	}
	System.out.println("==================================================================");
	System.out.println("=== Trying to close some ports");
	in.close();
	out.close();
	Time.delay(1);
	if (!ok) {
	    System.out.println("*** YARP seems broken.");
	    //diagnose();
	    System.exit(1);
	} else {
	    System.out.println("*** YARP seems okay!");
	}
    }

    private static String slashify(String s) {
	String r = s;
	if (s!=null) {
	    if (s.length()>0) {
		if (s.charAt(0)!='/' && s.charAt(0)!='.') {
		    r = "/" + s;
		}
	    }
	}
	return r;
    }

    /**
     * Send a message to the name server.
     * @param message text of the message to send (NAME_SERVER will be prepended)
     */
    public static void name(String message) {
	String result = "";
	try {
	    result = NameClient.getNameClient().send(message);
	} catch (IOException e) {
	    log.error("Problem communicating with nameserver");
	    System.exit(1);
	}
	System.out.println(result);
    }

    private static void name(String[] args) {
	StringBuffer cmd = new StringBuffer("NAME_SERVER");
	for (int i=1; i<args.length; i++){ 
	    cmd.append(" ");
	    cmd.append(args[i]);
	}
	name(cmd.toString());
    }

    /**
     * Request that an output port connect to an input port.
     * @source the name of an output port
     * @target the name of an input port
     * @carrier the name of the carrier to use
     */
    public static void connect(String source, String target, String carrier) {
	System.out.println("Connecting " + source + " to " + target);
	source = slashify(source);
	target = slashify(target);
	if (carrier!=null) {
	    target = "/" + carrier + ":/" + target;
	}
	try {
	    command(source,target);
	} catch(IOException e) {
	    log.error(e.toString());
	}
    }


    private static void connect(String[] args) {
	if (args.length<3) {
	    System.err.println("use as: <this program> connect /src /target");
	    System.exit(1);
	}
	String source = args[1];
	String target = args[2];
	String carrier = null;
	if (args.length>=4) {
	    carrier = args[3];
	}
	connect(source,target,carrier);
    }

    /**
     * Request that an output port disconnect from an input port.
     * @source the name of an output port
     * @target the name of an input port
     */
    public static void disconnect(String source, String target) {
	System.out.println("Disconnecting " + source + " from " + target);
	source = slashify(source);
	target = slashify(target);
	try {
	    command(source,"!"+target);
	} catch(IOException e) {
	    log.error(e.toString());
	}
    }


    private static void disconnect(String[] args) {
	if (args.length<3) {
	    System.err.println("use as: <this program> disconnect /src /target");
	    System.exit(1);
	}
	String source = args[1];
	String target = args[2];
	disconnect(source,target);
    }

    /**
     * Create a port to read from the standard input, convert that
     * input into a Bottle, and send it out.  The Bottles will
     * consist of an integer followed by a string.  The integer
     * indicates whether the "end-of-file" has been reached.  The
     * string is what was read from standard input.
     * @param name the name which which to register the port
     * @param targets an optional list of the names of ports to output to.
     */
    public static void write(String name, String[] targets) {
	OutputPort out = new OutputPort();
	out.creator(new BottleContent());
	out.register(name);
	for (int i=0; i<targets.length; i++) {
	    out.connect(targets[i]);
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

    private static void write(String[] args) {
	if (args.length<2) {
	    System.err.println("use as: <this program> write /name [/target]");
	    System.exit(1);
	}
	String name = args[1];
	write(name,subArray(args,2,args.length-2));
    }

    /**
     * Report where the name server is believed to be.
     */
    public static void where() {
	Address address = NameClient.getNameClient().getAddress();
	File file = NameClient.getNameClient().getConfigFile();
	if (address!=null) {
	    System.out.println("Name server is available at ip " +
			       address.getName() + " port " + address.getPort());
	} else {
	    System.out.println("Address of name server is not configured");
	}
	System.out.println("This is configured in file " +
			   file);
	System.out.println("You can change the directory where this configuration file is stored\nwith the YARP_ROOT environment variable.");
    }

    /**
     * Report the YARP version.
     */
    public static void version() {
	System.out.println("YARP network version 2.0");
    }

    /**
     * The standard main method for the YARP utility.
     * @param args Command line arguments
     */
    public static void main(String[] args) {
	//System.out.println("256 is " + NetType.netInt(new byte[] {0,1,0,0}));
	//System.out.println("-1 is " + NetType.netInt(new byte[] {-1,-1,-1,-1}));
	//System.exit(1);
	if (args.length>0) {
	    String arg = args[0];
	    if (arg.equals("verbose")) {
		log.showAll();
		log.println("In verbose mode");
		String args2[] = new String[args.length-1];
		if (args2.length>0) {
		    System.arraycopy(args,1,args2,0,args2.length);
		    args = args2;
		}
	    }
	}
	if (args.length>0) {
	    log.println("first arg is " + args[0]);
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
	    if (mode.equals("name")) {
		name(args);
	    }
	    if (mode.equals("remote")) {
		remote(args);
	    }
	    if (mode.equals("where")) {
		where();
	    }
	    if (mode.equals("version")) {
		version();
	    }
	    if (mode.equals("check")) {
		check();
	    }
	} else {
	    System.err.println("here are ways to use this program:");
	    System.err.println("  <this program> read /name");
	    System.err.println("  <this program> write /name [/target]");
	    System.err.println("  <this program> connect /source /target [carrier]");
	    System.err.println("     carrier can be: " +
			       NetType.addStrings(Carriers.collectAccepts()));
	    System.err.println("  <this program> disconnect /source /target");
	    System.err.println("  <this program> server");
	    System.err.println("  <this program> where");
	    System.err.println("  <this program> version");
	    System.err.println("  <this program> name {arguments}");
	}
    }
}

