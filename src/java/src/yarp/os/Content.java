
package yarp.os;

import java.io.*;

public interface Content extends ContentCreator {
    public void read(BlockReader reader) throws IOException;
    public void write(BlockWriter writer) throws IOException;
    public void release();
    public Object object();
    public Content create();
}
