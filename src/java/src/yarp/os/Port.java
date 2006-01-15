
package yarp.os;

import java.io.*;
import java.nio.channels.*;
import java.net.*;
import java.util.*;

public interface Port {
    void register(String name);
    void close();
    //void connect(String str) throws IOException;
    //void send(Content content);
}
