
package yarp.os;

import java.io.*;
import java.util.*;

public interface ShiftStream {
    public InputStream getInputStream() throws IOException;
    public OutputStream getOutputStream() throws IOException;
    public void close() throws IOException;
}

