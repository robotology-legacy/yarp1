
package yarp.os;

import java.io.*;

interface ProtocolHandler {
    public void read(Protocol proto) throws IOException;
    public void write(Protocol proto) throws IOException;
}

