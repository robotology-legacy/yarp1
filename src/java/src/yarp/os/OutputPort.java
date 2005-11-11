
package yarp.os;

import java.lang.*;

public class OutputPort implements ProtocolHandler {
    private Port port;
    public void register(String name) {
	Address server = NameClient.getNameClient().register(name);
	port = new Port(server);
	port.setHandler(this);
	//content = creator.create();
	port.start();
    }

    public void read(Protocol proto) {
    }
    
    public void write(Protocol proto) {
    }

    public Object content() {
	return null;
    }
}
