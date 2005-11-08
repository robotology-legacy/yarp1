import java.io.*;
import java.net.*;
import java.util.*;

public class Portlet extends Thread {
    private Port owner;
    private ShiftStream shift;

    public Portlet(Port owner, ShiftStream shift) {
	this.owner = owner;
	this.shift = shift;
    }


    public void run() {
	try {
	// hand over control of this socket
	Protocol protocol = new Protocol(shift);

	protocol.expectHeader();
	protocol.respondToHeader();

	while (protocol.isOk()) {
	    protocol.expectIndex();
	    if (!protocol.respondToIndex()) { 
		System.out.println("Protocol problem");
		System.exit(1); 
	    }

	    if (protocol.getSize()>=8) {
		byte[] b = protocol.expectBlock(8);
		int cmd = (int)b[5];
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
		    case '/':
			System.out.println("Should send output to " + 
					   new String (arg));
			Address add = NameClient.getNameClient().query(new String(arg));
			if (add!=null) {
			    owner.addConnection(new Connection(add));
			}
			break;
		    case 'd':
			System.out.println("Someone sent me data!");
			break;
		    }
		}
	    }
	    protocol.expectBlock(protocol.getSize());

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
