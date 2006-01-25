
package yarp.os;

import java.io.*;

interface ProtocolHandler {
    public void read(BlockReader reader) throws IOException;
    //public void write(Protocol proto) throws IOException;
}

