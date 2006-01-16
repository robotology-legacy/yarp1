package yarp.os;

import java.io.*;


/**
 * Read from an input port.  This is the interface available when
 * reconstructing an object from a description.
 * The underlying protocol is hidden,
 * except that the object may optionally interpret a more
 * human-friendly form during text-mode connections.
 */
public interface BlockReader {

    public boolean expectBlock(byte[] b, int len) throws IOException;
    public byte[] expectBlock(int len) throws IOException;
    public String expectString(int len) throws IOException;
    public String expectLine() throws IOException;
    public int expectInt() throws IOException;

    public boolean isTextMode() throws IOException;
}
