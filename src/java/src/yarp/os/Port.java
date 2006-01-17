
package yarp.os;

/**
 * Network-visible object.
 */
public interface Port {
    void register(String name);
    String name();
    void close();
}
