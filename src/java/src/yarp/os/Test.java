
package yarp.os;

import java.io.*;
import java.util.*;

class Test {
    public static void oldTest() {
	try {
	NameClient nc = NameClient.getNameClient();

	String name = "/read";
	Address add = nc.query(name);

	if (add==null) {
	    System.out.println("Cannot find port " + name + " to write to");
	    System.exit(1);
	}

	Connection c = new Connection(add,"/fake",name);
	
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
	op.register("/op");
	op.creator(new BottleContent());

	op.register("tcp://op");
	op.connect("mcast://read");
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


    public static void main(String[] args) {
	if (args.length>=1) {
	    YarpClient.main(args);
	} else {
	    //oldTest();
	    outPortTest();
	    //inPortTest();
	}
    }
}
