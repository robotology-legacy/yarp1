
package yarp.os;

import java.io.*;
import java.net.*;

interface ShiftStream {

    public InputStream getInputStream() throws IOException;
    public OutputStream getOutputStream() throws IOException;

    //public Address getAddress() throws IOException;

    public Address getLocalAddress() throws IOException;
    public Address getRemoteAddress() throws IOException;

    //public void open(String carrier, Address address, Carrier prev) 
    //throws IOException;

    public void open(Address address, ShiftStream previous) 
	throws IOException;

    public String getName();

    public Socket takeSocket();

    public void close() throws IOException;

}

