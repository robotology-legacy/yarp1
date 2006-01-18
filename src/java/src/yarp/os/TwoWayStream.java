
package yarp.os;

import java.io.*;

interface TwoWayStream {
    public InputStream getInputStream() throws IOException;
    public OutputStream getOutputStream() throws IOException;

    public Address getLocalAddress() throws IOException;
    public Address getRemoteAddress() throws IOException;

    public void close() throws IOException;

 }
