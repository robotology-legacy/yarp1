
package yarp.os;

/**
 * An abstraction for a location within the yarp system.  Typically
 * this is just a hostname, a port number, and the network protocol.
 * This may need to be extended for other systems, e.g. QNX.
 *
 */
public class Address {

    public Address(String name, int port) {
	this.name = name;
	this.port = port;
	this.carrier = "tcp";
    }

    public Address(String name, int port, String carrier) {
	this.name = name;
	this.port = port;
	this.carrier = carrier;
    }

    public String getName() { return name; }

    public int getPort() {return port; }

    public String getCarrier() { return carrier; }

    public String toString() { return carrier + "://" + name + ":" + port;  }

    private String name;
    private int port;
    private String carrier;
}
