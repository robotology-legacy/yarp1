
package yarp.os;

public interface ProtocolHandler {
    public void read(Protocol proto);
    public void write(Protocol proto);
}

