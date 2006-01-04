
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;


public class Connection {
    private Address address;
    private Protocol proto;
    private Socket socket = null;
    private String fromKey = null;
    private String toKey = null;

    public Connection(Address address, 
		      String fromKey, 
		      String toKey) {
	this.address = address;
	this.fromKey = fromKey;
	this.toKey = toKey;

	try {
	    socket = new Socket(address.getName(),address.getPort());
	} catch (UnknownHostException e) {
	    System.err.println("Don't know about host " + address.getName());
	    System.exit(1);
	} catch (IOException e) {
	    System.err.println("Couldn't get I/O for the connection to " + address.getName());
	    System.exit(1);
	}
	    
	if (socket==null) {
	    System.exit(1);
	}
	    
	try {
	    
	    proto = new Protocol(new SocketShiftStream(socket));
	    
	    proto.setSender(fromKey);
	    proto.setRawProtocol(NameClient.getProtocolPart(toKey));
	    proto.sendHeader();
	    proto.expectReplyToHeader();
	    System.out.println("Connection created ok");
	} catch (IOException e) {
	    System.err.println("Problem creating connection to " + address);
	}
    }

    public String getFromName() {
	return fromKey;
    }

    public String getToName() {
	return toKey;
    }

    public void close() {
	proto.close();
    }

    private byte[] netInt(int x) {
	byte b[] = new byte[4];
	for (int i=0; i<4; i++) {
	    int bi = x%256;
	    b[i] = (byte)bi;
	    x /= 256;
	}
	return b;
    }

    private byte[] netString(String s) {
	byte b[] = new byte[s.length()+1];
	for (int i=0; i<s.length(); i++) {
	    b[i] = (byte)s.charAt(i);
	}
	b[s.length()] = '\0';
	return b;
    }
    

    private byte[] append(byte[] b1, byte[] b2) {
	byte[] b3 = new byte[b1.length+b2.length];

	System.arraycopy (b1, 0, b3, 0, b1.length);
	System.arraycopy (b2, 0, b3, b1.length,
			  b2.length); 
	return b3;
    }

    public void write(String msg) throws IOException {
	write(msg,false);
    }

    // specialized for Bottle
    // not used any more, was just to understand things better
    public void write(String msg, boolean eof) throws IOException {

	InputStream in = null;
        OutputStream out = null;
	in = socket.getInputStream();
	out = socket.getOutputStream();
	

	out.write(new byte[] { 'Y', 'A', 10, 0, 0, 0, 'R', 'P' });
	out.write(new byte[] {2, 1,    
			      -128,-128,-128,-128,  
			      -128,-128,-128,-128});

	byte b2[] = {1,0,0,0, (byte)(eof?1:0),0,0,0};        // INT 0
	b2 = append(b2, new byte[] {5,0,0,0}); // STR
	b2 = append(b2,netInt(msg.length()+1));
	b2 = append(b2,netString(msg));

	byte b1[] = { 0,0,0,0,  '~', 'd', 0, 1};
	b1 = append(b1,new byte[] { 4,0,0,0 });
	b1 = append(b1,new byte[] { 'n','/','a',0 });
	b1 = append(b1,netInt(b2.length));

	byte b0[] = {};
	b0 = append(b0,netInt(b1.length)); // block 1 length
	b0 = append(b0,netInt(b2.length)); // block 2 length
	b0 = append(b0,netInt(0));  // response length
    

	out.write(b0);
	out.write(b1);
	out.write(b2);
	out.flush();
    }

    public void writeCommand(char ch, String str) throws IOException {

	proto.beginContent();

	proto.appendBlock(new byte[] { 0,0,0,0, '~', (byte)ch, 0, 1}); // data hdr
	if (str!=null) {
	    proto.appendString(str);
	}

	proto.endContent();
	proto.sendIndex();
	proto.sendContent();
    }

    public void write(Content content) throws IOException {

	proto.beginContent();
	proto.addContent(new byte[] { 0,0,0,0,  '~', 'd', 0, 1}); // data hdr
	content.write(proto);
	//content.addContent(proto.netInt(4));
	//content.addContent(proto.netString("nul"));
	proto.endContent();
	proto.sendIndex();
	proto.sendContent();
    }
}

