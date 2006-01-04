
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;

class BasicPortlet extends Portlet {
    private BasicPort owner;
    private ShiftStream shift;

    public BasicPortlet(BasicPort owner, ShiftStream shift) {
	this.owner = owner;
	this.shift = shift;
    }

    public void close() {
	System.out.println("Trying to halt portlet");
	try {
	    shift.close();
	} catch (IOException e) {
	    System.err.println("Problem while halting portlet");
	}
	interrupt();
    }

    public void run() {
	try {
	// hand over control of this socket
	Protocol protocol = new Protocol(shift);

	protocol.expectHeader();
	protocol.respondToHeader();

	while (protocol.isOk()) {
	    protocol.expectIndex();
	    System.out.println("got index, responding..");
	    if (!protocol.respondToIndex()) { 
		System.out.println("Protocol problem");
		System.exit(1); 
	    }
	    System.out.println("got index, responded.");
	    System.out.println("size is " + protocol.getSize());
	    if (protocol.getSize()>=8) {
		System.out.println("waiting for 8");
		byte[] b = protocol.expectBlock(8);
		System.out.println("got 8");
		int cmd = (int)b[5];
		System.out.println("command is " + cmd);
		byte[] arg = new byte[] {0};
		if (b[4]==(byte)'~') {
		    if (protocol.getSize()>0 && cmd==0) {
			arg = protocol.expectBlock(-1);
			if (cmd==0) {
			    cmd = ((int)arg[0]);
			    System.out.println("Subbing cmd " + cmd);
			}
		    }
		} else {
		    System.out.println("Bad message");
		}
		if (cmd!=0) {
		    System.out.println("Got command [" + ((char)cmd) + "]");
		    switch(cmd) {
		    case '!':
			String src = new String(arg);
			src = src.substring(1,src.length());
			System.out.println("Should disconnect output from " + 
					   src);
			owner.removeConnection(src);
			break;
		    case '/':
			System.out.println("Should send output to " + 
					   new String (arg));
			Address add = NameClient.getNameClient().query(new String(arg));
			if (add!=null) {
			    owner.addConnection(new Connection(add,owner.getPortName(),new String(arg)));
			}
			break;
		    case 'd':
			System.out.println("Someone sent me data!");
			owner.handleData(protocol);
			break;
		    }
		}
	    }
	    protocol.expectBlock(protocol.getSize());
	    protocol.respondToBlock();

	    System.out.println("Finished");
	}
	shift.close();

	//clientSocket.close();
	} catch (IOException e) {
	    System.out.println("Portlet IO shutdown");
	    // should call owner to forget me
	}
	System.out.println("Shutting down portlet");
	owner.removePortlet(this);
    }

}
