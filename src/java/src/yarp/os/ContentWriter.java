
package yarp.os;

import java.io.IOException;

interface ContentWriter {
    public void write(Content content, boolean tagAsData) throws IOException;
    public void close() throws IOException;
}

