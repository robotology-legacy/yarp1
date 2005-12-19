
package yarp.os;

import java.io.*;
import java.util.*;

public interface ShiftStream {
    public InputStream getInputStream() throws IOException;
    public OutputStream getOutputStream() throws IOException;
    public void close() throws IOException;
    public void becomeUdp(int remotePort) throws IOException;
    public void becomeMcast(Address address) throws IOException;
    public int getLocalPort() throws IOException;
}

