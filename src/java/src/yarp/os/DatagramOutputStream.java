
package yarp.os;

import java.io.*;
import java.net.*;

// this stream should be wrapped in a BufferedOutputStream or it 
// will be inefficient
class DatagramOutputStream extends OutputStream {
    private DatagramSocket dgram;
    private Address address;
    private DatagramPacket packet; // use later to optimize
    private int len;

    public DatagramOutputStream(DatagramSocket dgram, Address address,
				int len) {
	this.dgram = dgram;
	this.address = address;
	this.len = len;
    }

    public void write(int b) throws IOException {
	System.out.println("write 1 called");
	byte[] a = { ((byte)b) };
	write(a);
    }

    public void write(byte[] b, int offset, int len) throws IOException {
	System.out.println("write 2 called");
	SocketAddress sad = new InetSocketAddress(address.getName(),
						  address.getPort());
	DatagramPacket p = new DatagramPacket(b, offset, len, sad);
	System.out.println("Datagram output to " + address.getName() + " " + address.getPort());
	dgram.send(p);
    }

    public void write(byte[] b) throws IOException {
	System.out.println("write 3 called");
	write(b,0,b.length);
    }
}

