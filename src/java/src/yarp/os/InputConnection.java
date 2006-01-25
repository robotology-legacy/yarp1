
package yarp.os;

import java.io.*;

/**
 * This class represents the input side of a connection
 */

class InputConnection {
    private Face face;
    private Carrier carrier;

    public InputConnection(Face face) {
	this.face = face;
    }

    private void init() {
    }

}

