
package yarp.os;

import java.io.*;
import java.net.*;

class DatagramInputStream extends InputStream {
    private static Logger log = Logger.get();

    public static final int DGRAM_BUF_LEN = 512;

    private DatagramSocket dgram;
    private DatagramPacket packet; // use later to optimize
    private int len;
    private byte[] buf = new byte[DGRAM_BUF_LEN];
    private byte[] data;
    private int at = 0;
    private int avail = 0;


    public DatagramInputStream(DatagramSocket dgram, int len) {
	this.dgram = dgram;
	this.len = len;
    }

    public int readHelper() throws IOException {
	DatagramPacket recv = new DatagramPacket(buf, buf.length);
	dgram.receive(recv);
	data = recv.getData();
	log.println("Received: " + recv.getOffset() + " " + recv.getLength());
	at = recv.getOffset();
	avail = recv.getLength();
	return avail;
    }

    public int read() throws IOException {
	byte[] b = { 1 };
	int r = read(b);
	if (r>=0) { 
	    return b[0];
	}
	return -1;
    }

    public int available() throws IOException {
	if (avail==0) {
	    dgram.setSoTimeout(1);
	    readHelper();
	    dgram.setSoTimeout(0);
	}
	return avail;
    }

    public int read(byte[] b) throws IOException {
	return read(b,0,b.length);
    }

    public int read(byte[] b, int off, int len) throws IOException {
	int ct = 0;
	while (len>0) {
	    //log.println("udp read at " + at + " avail " + avail + " rel " + off + " " + len);
	    if (avail==0) {
		readHelper();
	    }
	    while (avail>0 && len>0) {
		b[off] = buf[at];
		at++;
		avail--;
		off++;
		len--;
		ct++;
	    }
	}
	//log.println("udp read done " + off + " " + len);
	return ct;
    }

}

