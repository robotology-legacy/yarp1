
package yarp.os;

import java.io.*;
import java.util.*;


/*
  implementation of some standard yarp client utilities
 */

class YarpClient {
    private static Logger log = Logger.get();

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
		List lst = bot.asList();
		boolean shown = false;
		if (lst.size()>=2) {
		    Object o = lst.get(0);
		    if (o instanceof Integer) {
			if (((Integer) o).intValue()==1) {
			    done = true;
			    shown = true;
			    // honor end-of-stream marker
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


    public static void command(String source, String cmd) throws IOException {
	command(source,cmd,'\0');
    }

    public static void command(String source, String cmd, char ch) throws IOException {
	try {
	    NameClient nc = NameClient.getNameClient();
	    
	    Address add = nc.query(source);
	    
	    if (add==null) {
		System.err.println("Cannot find port " + source);
		return;
	    }
	    
	    Connection c = new Connection(add,"external",source);
	    log.println("command is [" + cmd + "] and char is [" + ((int)ch) + "]");
	    c.writeCommand(ch,cmd);
	    c.close();
	} catch (IOException e) {
	    throw(new IOException("connection failed"));
	}
    }

    public static String slashify(String s) {
	String r = s;
	if (s!=null) {
	    if (s.length()>0) {
		if (s.charAt(0)!='/') {
		    r = "/" + s;
		}
	    }
	}
	return r;
    }

    public static void name(String[] args) {
	StringBuffer cmd = new StringBuffer("NAME_SERVER");
	for (int i=1; i<args.length; i++){ 
	    cmd.append(" ");
	    cmd.append(args[i]);
	}
	String result = "";
	try {
	    result = NameClient.getNameClient().send(cmd.toString());
	} catch (IOException e) {
	    log.error("Problem communicating with nameserver");
	    System.exit(1);
	}
	System.out.println(result);
    }

    public static void connect(String[] args) {
	if (args.length<3) {
	    System.err.println("use as: <this program> connect /src /target");
	    System.exit(1);
	}
	String source = args[1];
	String target = args[2];
	System.out.println("Connecting " + source + " to " + target);
	source = slashify(source);
	target = slashify(target);
	if (args.length>3) {
	    String carrier = args[3];
	    target = "/" + carrier + ":/" + target;
	}
	try {
	    command(source,target);
	} catch(IOException e) {
	    log.error(e.toString());
	}
    }

    public static void disconnect(String[] args) {
	if (args.length<3) {
	    System.err.println("use as: <this program> disconnect /src /target");
	    System.exit(1);
	}
	String source = args[1];
	String target = args[2];
	System.out.println("Disconnecting " + source + " from " + target);
	source = slashify(source);
	target = slashify(target);
	try {
	    command(source,"!"+target);
	} catch(IOException e) {
	    log.error(e.toString());
	}
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

    public static void where() {
	Address address = NameClient.getNameClient().getAddress();
	System.out.println("Name server is available at ip " +
			   address.getName() + " port " + address.getPort());
    }

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
	    if (mode.equals("where")) {
		where();
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
	    System.err.println("  <this program> name {arguments}");
	}
    }
}

