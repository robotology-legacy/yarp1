
package yarp.os;

import java.io.*;

interface InputProtocol {

    void initiate(String name) throws IOException;
    void close() throws IOException;

    BlockReader beginRead() throws IOException;
    void endRead() throws IOException;

    Route getRoute(); // not final until initiate is called

    OutputStream getReplyStream() throws IOException;

    // silly TelnetPort dependency, due for removal
    InputStream getInputStream() throws IOException;
    Address getRemoteAddress() throws IOException;
}

