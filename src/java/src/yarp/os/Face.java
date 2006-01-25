
package yarp.os;

import java.io.*;

interface Face {

    void open(Address address) throws IOException;
    void close() throws IOException;

    InputProtocol read() throws IOException;

    boolean isClosed();
}

