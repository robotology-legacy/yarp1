
package yarp.os;

public class Address {
    public Address(String name, int port, String carrier) {
	this.name = name;
	this.port = port;
	this.carrier = carrier;
    }

    public Address(String name, int port) {
	this.name = name;
	this.port = port;
	this.carrier = "udp";
    }

    public String getName() { return name; }
    public int getPort() {return port; }
    public String getCarrier() { return carrier; }

    public String toString() { return carrier + ":/" + name + ":" + port;  }

    private String name;
    private int port;
    private String carrier;
}
