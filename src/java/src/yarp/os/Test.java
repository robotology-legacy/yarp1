
package yarp.os;

import java.io.*;

class Test {
    public static void oldTest() 
	throws IOException, InterruptedException {
	NameClient nc = NameClient.getNameClient();

	Address add = nc.query("/read");

	if (add==null) {
	    System.out.println("Cannot find port to write to");
	    System.exit(1);
	}

	Connection c = new Connection(add);
	
	c.send("peter piper");
	c.send("rules");

	/*
	Address server = nc.register("/boo");
	Port p = new Port(server);
	p.run();
	*/

    }


    public static void main(String[] args) {
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
