package yarp.os;

import java.io.*;

public interface BlockWriter {
    public void appendBlock(byte[] data);
    public void appendInt(int data);
    public void appendString(String data);
    public void appendLine(String data);
    public boolean isTextMode() throws IOException;
}

