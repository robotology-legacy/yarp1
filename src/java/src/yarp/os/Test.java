
package yarp.os;

import java.io.*;

class Test {
    public static void oldTest() {
	try {
	NameClient nc = NameClient.getNameClient();

	Address add = nc.query("/read");

	if (add==null) {
	    System.out.println("Cannot find port to write to");
	    System.exit(1);
	}

	Connection c = new Connection(add);
	
	//c.send("peter piper");
	//c.send("rules");

	BottleContent bot = new BottleContent();
	Bottle b = (Bottle)bot.content();

	b.clear();
	b.add(new Integer(0));
	b.add("Hello");
	System.out.println("Preparing to send bottle...");
	c.send(bot);
	System.out.println("bottle sent");

	b.clear();
	b.add(new Integer(0));
	b.add("There");
	c.send(bot);

	b.clear();
	b.add(new Integer(0));
	b.add("My friend");
	c.send(bot);


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


    public static void main(String[] args) {
	oldTest();

	InputPort ip = new InputPort();
	ip.create(new BottleContent());
	ip.register("/foo");
	while (ip.read()) {
	    System.out.println("I read something!");
	    Bottle bot = (Bottle) ip.content();
	    if (bot!=null) {
		System.out.println(bot.asList());
	    }
	}
    }
}
