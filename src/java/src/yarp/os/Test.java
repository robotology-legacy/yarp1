
package yarp.os;

import java.io.*;


class Test {
    static Logger log = new Logger("test: ",null);

    public static void oldTest() {
	try {
	NameClient nc = NameClient.getNameClient();

	String name = "/read";
	Address add = nc.query(name);

	if (add==null) {
	    System.out.println("Cannot find port " + name + " to write to");
	    System.exit(1);
	}

	Connection c = new Connection(add,"tcp","/fake",name,true);
	
	//c.send("peter piper");
	//c.write("rules");

	OutputPort op = new OutputPort();
	op.register("/op");
	op.connect("/read2");

	BottleContent bot = new BottleContent();
	Bottle b = (Bottle)bot.object();

	b.clear();
	b.add(new Integer(0));
	b.add("Hello");
	System.out.println("Preparing to send bottle...");
	System.out.println("requesting send for: " + b);
	c.write(bot);
	op.write(bot);
	System.out.println("bottle sent");

	b.clear();
	b.add(new Integer(0));
	b.add("There");
	System.out.println("requesting send for: " + b);
	c.write(bot);

	b.clear();
	b.add(new Integer(0));
	b.add("My friend");
	System.out.println("requesting send for: " + b);
	c.write(bot);


	b.clear();
	b.add(new Integer(0));
	b.add("My very good friend");
	System.out.println("requesting send for: " + b);
	c.write(bot);


	b.clear();
	b.add(new Integer(0));
	b.add("My very very good friend");
	System.out.println("requesting send for: " + b);
	c.write(bot);


	Time.delay(1);

	System.out.println("FINISHED!");

	/*
	Address server = nc.register("/boo");
	Port p = new Port(server);
	p.run();
	*/
	} catch (Exception e) {
	    System.out.println("failed");
	}
    }


    public static void outPortTest() {
	OutputPort op = new OutputPort();
	//op.register("/op");
	op.register("/write");
	op.creator(new BottleContent());

	//op.connect("mcast://read");
	//op.connect("/read");

	
	Bottle b = (Bottle)op.content();

	b.clear();
	b.add(new Integer(0));
	b.add("Hello");
	System.out.println("requesting send for: " + b);
	op.write();
	System.out.println("bottle sent");

	Time.delay(1);

	for (int i = 0; i<2000; i++) {
	    b = (Bottle)op.content();
	    b.clear();
	    b.add(new Integer(0));
	    b.add("My Friend " + i);
	    System.out.println("requesting send for: " + b);
	    op.write();
	    System.out.println("bottle sent");
	    Time.delay(1);
	}

	
	System.out.println("starting close...");
	op.close();
	System.out.println("finished close...");
    }

    public static void inPortTest() {
	InputPort ip = new InputPort();
	ip.creator(new BottleContent());
	ip.register("/read");
	int ct = 0;
	while (ip.read()&&ct<3) {
	    System.out.println("I read something!");
	    Bottle bot = (Bottle) ip.content();
	    if (bot!=null) {
		System.out.println(bot.asList());
		ct++;
	    }
	}
	ip.close();
    }


    public static void parseTest() {

	StreamTokenizer stream =null;
	try{
	    stream = new StreamTokenizer( new 
					  FileReader("customers.txt"));
	    while (true) {
		
		int token = stream.nextToken();
		if (token == StreamTokenizer.TT_EOF)
		    break;
		if (token == StreamTokenizer.TT_WORD) {
		    System.out.println("Got the string: " + 
				       stream.sval);
		}
		else if (token == StreamTokenizer.TT_NUMBER) {
		    System.out.println("Got the number: " + 
				       stream.nval);
		} else {
		    System.out.println("Got other: " + 
				       stream.sval);
		}

	    }
	}catch (Exception e){
	    System.out.println("Can't read customers.txt: " + 
			       e.toString());
	}
	finally{
	    try{
		//stream.close();
	    }catch(Exception e){e.printStackTrace();}          
	}
    }

    
    public static void conTest() {
	Logger.get().showAll();
	log.showAll();
	try {
	    final Address addr = new Address("localhost",9000,"tcp");
	    Face face = Carriers.listen(addr);

	    Thread t1 = new Thread() {
		    public void run() {
			try {
			    log.info("delaying start of writer");
			    Time.delay(1);
			    log.info("start writer");
			    //Address addr2 = 
				//NameClient.getNameClient().query("/read");
			    OutputProtocol out = Carriers.connect(addr);

			    // At this point we have a connection.
			    // A header handshake is expected.
			    out.initiate("yarp-client","yarp-server","mcast");
			    log.info("WRITE route is " + out.getRoute());
			    log.info("handshaking done");

			    //ContentWriter writer = new SlowWriter(out);
			    CommandContent cmd = new CommandContent('d',null);
			    BottleContent bot = new BottleContent();
			    ((Bottle)(bot.object())).fromString("0  \"hello\"");
			    //writer.write(bot,true);
			    BlockWriter writer = out.beginWrite();
			    log.assertion(writer!=null);
			    cmd.write(writer);
			    bot.write(writer);
			    out.endWrite();
			    Time.delay(1);
			    log.info("close writer");
			    out.close();
			} catch (IOException e) {
			    log.info("inner conTest exception " + e);
			}
		    }
		};
	    t1.start();
	    log.info("waiting for connection");
	    InputProtocol in = face.read();
	    log.info("start reader");
	    in.initiate("yarp-server");
	    log.info("READ route is " + in.getRoute());
	    log.info("reader got header");
	    BlockReader reader = in.beginRead();
	    CommandContent cmd = new CommandContent();
	    BottleContent bot = new BottleContent();
	    cmd.read(reader);
	    log.info("reader got command");
	    bot.read(reader);
	    in.endRead();
	    log.info("reader got bottle");
	    log.info("got " + ((String)cmd.object()) + " -- " + 
		     ((Bottle)bot.object()));
	    log.info("close reader");
	    in.close();

	    face.close();
	} catch (IOException e) {
	    log.info("conTest exception " + e);
	}
    }

    public static void testMcastServer() {
	Logger.get().showAll();
	log.showAll();

	FallbackServer fb = new FallbackServer(new YarpServer());
	fb.run();
	/*
	final Address addr = new Address("224.2.1.1",9999,"mcast");
	try {
	    Carrier car = new McastCarrier();
	    car.start(addr);
	    InputStream in = car.getStreams().getInputStream();
	    String txt = NetType.readLine(in);
	    log.info("got " + txt);
	} catch (IOException e) {
	    log.info("mcast turner " + e);
	}
	*/
    }

    public static void testMcastClient() {
	log.info("mcast client");
	Logger.get().showAll();
	log.showAll();
	final Address addr = FallbackServer.getAddress();
	try {
	    Carrier car = new McastCarrier();
	    car.start(addr);
	    OutputStream out = car.getStreams().getOutputStream();
	    InputStream in = car.getStreams().getInputStream();
	    out.write(NetType.netString("NAME_SERVER query root\n"));
	    out.flush();
	    for (int i=0; i<5; i++) {
		String txt = NetType.readLine(in);
		log.info("got " + txt);
	    }
	} catch (IOException e) {
	    log.info("mcast turner " + e);
	}
    }

    public static void main(String[] args) {
	if (args.length>=1) {
	    if (args[0].equals("test")) {
		if (args.length>=2) {
		    if (args[1].equals("server")) {
			testMcastServer();
		    } else {
			testMcastClient();
		    }
		}
	    } else {
		YarpClient.main(args);
	    }
	} else {
	    YarpClient.main(args);
	}
	//parseTest();
	//YarpClient.main(args);
	//oldTest();
	//outPortTest();
	//inPortTest();
    }
}
