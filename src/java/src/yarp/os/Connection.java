
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;


public class Connection {
    private Address address;
    private Socket socket = null;

    public Connection(Address address) {
	this.address = address;
    }


    public boolean initiate() throws IOException {
	if (socket==null) {
	    try {
		socket = new Socket("localhost",10001);
	    } catch (UnknownHostException e) {
		System.err.println("Don't know about host " + address.getName());
		System.exit(1);
	    } catch (IOException e) {
		System.err.println("Couldn't get I/O for the connection to " + address.getName());
		System.exit(1);
	    }
	
	    if (socket==null) {
		return false;
	    }
	    
	    InputStream in = null;
	    OutputStream out = null;
	    in = socket.getInputStream();
	    out = socket.getOutputStream();
	    
	    {
		byte b[] = { 'Y', 'A', 0x64, 0x1e, 0, 0, 'R', 'P' };
		out.write(b);
		out.flush();
	    }
	    
	    {
		byte b[] = {4, 0, 0, 0};
		out.write(b);
		out.flush();
	    }
	    
	    {
		byte b[] = {'/', 'b', 'a', 'r', 0};
		out.write(b);
		out.flush();
	    }
	    
	    {
		byte b[] = new byte[8];
		in.read(b);
		//System.out.println(b[0]=='Y');
		int port = b[2]+256*b[3];
		//System.out.println("Port number is " + port);
	    }
	    
	    try {
		Thread.sleep(1000);
	    } catch (Exception e) {
		System.out.println("sleep failed");
		return false;
	    }
	}
	return true;
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

    public void send(String msg) throws IOException {
	send(msg,false);
    }

    public void send(String msg, boolean eof) throws IOException {

	initiate();

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
}

