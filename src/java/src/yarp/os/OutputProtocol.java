
package yarp.os;

import java.io.*;

interface OutputProtocol {

    void initiate(String src, String dest, String carrier) throws IOException;
    void close() throws IOException;

    BlockWriter beginWrite() throws IOException;
    void endWrite() throws IOException;

    Route getRoute(); // not final until initiate is called
}

