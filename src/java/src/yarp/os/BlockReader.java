package yarp.os;

import java.io.*;


/**
 * Write to an output port.
 */
public interface BlockReader {


    public boolean expectBlock(byte[] b, int len) throws IOException;


    public byte[] expectBlock(int len) throws IOException;


    public String expectString(int len) throws IOException;


    public int expectInt() throws IOException;
}
