
package yarp.os;

import java.io.*;

/**
 * Proxy for describing particular types of user data.
 * Must be able to read and write descriptions, and create
 * new object instances (e.g. for buffered input ports).
 */
public interface Content extends ContentCreator {
    /**
     * Write an object to a port.
     */
    public void read(BlockReader reader) throws IOException;


    /**
     * Read an object from a port.
     */
    public void write(BlockWriter writer) throws IOException;

    /**
     * Access an object being read/written.
     */
    public Object object();

    /**
     * Create another proxy for reading/writing objects.
     */
    public Content create();
}
