
package yarp.os;

/**
 * Network-visible object.
 */
public interface Port {
    void register(String name);
    void close();
}
