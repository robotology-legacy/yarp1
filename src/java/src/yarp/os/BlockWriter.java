package yarp.os;

import java.io.*;

/**
 * Write to an output port.  This is the interface available when
 * describing an object.  The underlying protocol is hidden,
 * except that the object may optionally be described in a more
 * human-friendly form for text-mode connections.
 */
public interface BlockWriter {
    public void appendBlock(byte[] data);
    public void appendInt(int data);
    public void appendString(String data);
    public void appendLine(String data);

    public boolean isTextMode() throws IOException;
}

