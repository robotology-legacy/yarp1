
package yarp.os;

public class Address {
    public Address(String name, int port) {
	this.name = name;
	this.port = port;
    }

    public String getName() { return name; }
    public int getPort() {return port; }

    public String toString() { return name + ":" + port; }

    private String name;
    private int port;
}
