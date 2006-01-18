
package yarp.os;

import java.io.*;
import java.net.*;

/**
 * A subset of Carrier
 */
interface ShiftStream {

    public InputStream getInputStream() throws IOException;
    public OutputStream getOutputStream() throws IOException;

    public Address getLocalAddress() throws IOException;
    public Address getRemoteAddress() throws IOException;

    public void open(Address address, ShiftStream previous) 
	throws IOException;

    public void close() throws IOException;

    public String getName();

    // pending - find a way to get rid of this
    public Socket takeSocket();
}

