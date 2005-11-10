
package yarp.os;

import java.io.*;

class Test {
    public static void main(String[] args) throws IOException {
	NameClient nc = new NameClient(new Address("localhost",10000));
	Address add = nc.query("/read");

	if (add==null) {
	    System.out.println("Cannot find port to write to");
	    System.exit(1);
	}

	Connection c = new Connection(add);
	
	c.send("peter piper");
	c.send("rules");
	
	Address server = nc.register("/boo");
	Port p = new Port(server);
	p.run();
    }
}
