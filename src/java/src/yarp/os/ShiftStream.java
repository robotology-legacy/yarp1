
package yarp.os;

import java.io.*;
import java.util.*;

interface ShiftStream {
    public InputStream getInputStream() throws IOException;
    public OutputStream getOutputStream() throws IOException;
    public void close() throws IOException;

    public void become(String carrier, Address address) throws IOException;
    public Address getAddress() throws IOException;

    public Carrier chooseCarrier(String name);
    public Carrier chooseCarrier(int specifier);
    public Carrier chooseCarrier(byte[] header);
    public void setIncoming(Carrier carrier);
}

