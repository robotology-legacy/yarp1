
package yarp.os;

import java.io.*;
import java.net.*;

// this stream should be wrapped in a BufferedOutputStream or it 
// will be inefficient
class DatagramOutputStream extends OutputStream {
    private static Logger log = Logger.get();

    private DatagramSocket dgram;
    private Address address;
    private DatagramPacket packet; // use later to optimize
    private int len;
    private boolean dummy = false;

    public DatagramOutputStream(DatagramSocket dgram, Address address,
				int len, boolean dummy) {
	this.dgram = dgram;
	this.address = address;
	this.len = len;
	this.dummy = dummy;
    }

    public void write(int b) throws IOException {
	log.println("write 1 called");
	byte[] a = { ((byte)b) };
	write(a);
    }

    public void write(byte[] b, int offset, int len) throws IOException {
	log.println("write 2 called");
	SocketAddress sad = new InetSocketAddress(address.getName(),
						  address.getPort());
	DatagramPacket p = new DatagramPacket(b, offset, len, sad);
	log.println("Datagram output to " + address.getName() + " " + address.getPort());
	if (!dummy) {
	    dgram.send(p);
	} else {
	    log.println("Not writing to DatagramOutputStream");
	}
    }

    public void write(byte[] b) throws IOException {
	log.println("write 3 called");
	write(b,0,b.length);
    }
}

