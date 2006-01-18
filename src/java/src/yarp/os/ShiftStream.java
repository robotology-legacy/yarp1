
package yarp.os;

import java.io.*;
import java.net.*;

/**
 * A subset of Carrier
 */
interface ShiftStream extends TwoWayStream {

    public void open(Address address, ShiftStream previous) 
	throws IOException;

    public String getName();

    public TwoWayStream takeStreams();
}

