
package yarp.os;

public interface Content extends ProtocolHandler {
    public void read(Protocol proto);
    public void write(Protocol proto);
    public void release();
    public Object content();
}
