
package yarp.os;

import java.io.*;
import java.net.*;

/**
 * Classes implementing this interface can be hollowed out,
 * and have their streams "borrowed"
 */
interface ShiftStream extends TwoWayStream {

    // shift streams can be moved around
    public TwoWayStream takeStreams();
}

