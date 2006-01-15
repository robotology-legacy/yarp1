
package yarp.os;

/**
 * An abstraction for a location within the yarp system.  This may be
 * just a hostname, a port number, and the network protocol.
 * This may need to be extended for other systems, e.g. QNX.
 * Addresses are immutable.
 *
 */
public class Address {

    /**
     * Simplest constructor.  The simplest type of address is just a 
     * machine name and a port, and an assumed protocol of tcp used
     * to communicate with that port.
     * @name Machine name - could be a hostname or ip address.
     * @port Port number for socket communication.
     */
    public Address(String name, int port) {
	this.name = name;
	this.port = port;
	this.carrier = "tcp";
    }

    /**
     * Constructor with explicit protocol.
     * @name Machine name - could be a hostname or ip address.
     * @port Port number for socket communication.
     * @carrier The raw protocol used for communication.
     */
    public Address(String name, int port, String carrier) {
	this.name = name;
	this.port = port;
	this.carrier = carrier;
    }

    /**
     * Constructor with explicit protocol and registered name.
     * @name Machine name - could be a hostname or ip address.
     * @port Port number for socket communication.
     * @carrier The raw protocol used for communication.
     * @regName A name associated with this address in the global name server.
     */
    public Address(String name, int port, String carrier, String regName) {
	this.name = name;
	this.port = port;
	this.carrier = carrier;
	this.regName = regName;
    }

    /**
     * Get machine name.
     * @return Machine name - could be a hostname or ip address.
     */
    public String getName() { return name; }

    /**
     * Get port number.
     * @return Port number for socket communication.
     */
    public int getPort() {return port; }

    /**
     * Get protocol.
     * @return The raw protocol used for communication.
     */
    public String getCarrier() { return carrier; }

    /**
     * Get registered name.
     * @return The name associated with this address in the global name server.
     */
    public String getRegName() { return regName; }

    /**
     * Render address textually, e.g. as something like udp://127.0.0.1:10001
     * (host 127.0.0.1, port 10001, protocol udp)
     * @return Textual representation of address.
     */
    public String toString() { return carrier + "://" + name + ":" + port;  }

    private String name;
    private int port;
    private String carrier;
    private String regName;
}
