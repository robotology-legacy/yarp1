
package yarp.os;

import java.io.*;

interface ProtocolHandler {
    public boolean read(BlockReader reader) throws IOException;
    //public void write(Protocol proto) throws IOException;

    //public boolean close() throws IOException;
}

