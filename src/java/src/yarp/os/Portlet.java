
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;

abstract class Portlet extends Thread {

    public abstract void close();

    public abstract void run();

}
