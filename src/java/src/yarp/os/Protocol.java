
package yarp.os;

import java.io.*;
import java.util.*;

/*
  Note - have to think about how to adapt this to
  work when part of protocol allows switching to a
  different network connection. 
  Probably have to put in some indirection - see
  ShiftStream.
 */

public class Protocol {
    private ShiftStream shift;
    private InputStream in;
    private OutputStream out;
    private boolean gotHeader = false;

    private ArrayList inputLen = new ArrayList();
    private ArrayList outputLen = new ArrayList();
    private int messageLen = 0;
    private boolean ok = true;
    private String senderName = "null";
    private ArrayList content = new ArrayList();

    public Protocol(ShiftStream shift) throws IOException {
	this.shift = shift;
	this.in = shift.getInputStream();
	this.out = shift.getOutputStream();
    }

    public void setSender(String name) {
	senderName = name;
    }

    private int readFull(byte[] b) throws IOException {
	int off = 0;
	int fullLen = b.length;
	int remLen = fullLen;
	int result = 1;
	while (result>0&&remLen>0) {
	    result = in.read(b,off,remLen);
	    //System.out.println("Read " + off + " " + remLen + " " + result);
	    if (result>0) {
		remLen -= result;
		off += result;
	    }
	}
	return (result<0)?result:fullLen;
    }

    public int netInt(byte[] b) {
	int x = 0;
	for (int i=b.length-1; i>=0; i--) {
	    x *= 256;
	    x += (int)b[i]; // warning, may have sign trouble
	}
	return x;
    }

    public byte[] netInt(int x) {
	byte b[] = new byte[4];
	for (int i=0; i<4; i++) {
	    int bi = x%256;
	    b[i] = (byte)bi;
	    x /= 256;
	}
	return b;
    }

    public byte[] netString(String s) {
	byte b[] = new byte[s.length()+1];
	for (int i=0; i<s.length(); i++) {
	    b[i] = (byte)s.charAt(i);
	}
	b[s.length()] = '\0';
	return b;
    }

    private boolean sendProtocolSpecifier() throws IOException {
	byte b[] = { 'Y', 'A', 0x64, 0x1e, 0, 0, 'R', 'P' };
	out.write(b);
	out.flush();
	return true;
    }

    // part of header
    private boolean expectProtocolSpecifier() throws IOException {
	// expect protocol specifying header.
	// assume for now it is TCP.

	byte b[] = new byte[8];
	readFull(b);
	if (b[0] != (byte)'Y') {
	    System.out.println("Garbage message");
	    return false;
	}
	gotHeader = true;
	// should do something with it!

	return true;
    }

    private boolean sendSenderSpecifier() throws IOException {
	out.write(netInt(senderName.length()+1));
	out.write(netString(senderName));
	out.flush();
	return true;
    }

    // part of header
    private boolean expectSenderSpecifier() throws IOException {
	int len = 0;
	{
	    // expect an ID string length -- an integer.
	    
	    byte b[] = new byte[4];
	    readFull(b);
	    len = netInt(b);
	}
	if (len>1000) len = 1000;
	if (len<1) len = 1;
	{
	    // expect a null-terminated string
	    byte b[] = new byte[len];
	    readFull(b);
	    System.out.println("name is " + new String(b));
	}
	return true;
    }

    public boolean sendHeader() throws IOException {
	sendProtocolSpecifier();
	sendSenderSpecifier();
	return true;
    }

    public boolean expectHeader() throws IOException {
	ok = true;
	inputLen = new ArrayList();
	outputLen = new ArrayList();
	messageLen = 0;

	if (!expectProtocolSpecifier()) { ok=false; return false; }
	if (!expectSenderSpecifier()) { ok=false; return false; }
	return true;
    }

    public boolean expectReplyToHeader() throws IOException {
	byte b[] = new byte[8];
	in.read(b);
	//System.out.println(b[0]=='Y');
	int port = b[2]+256*b[3];
	//System.out.println("Port number is " + port);
	if (b[0]!='Y') {
	    throw new IOException();
	}
	// For TCP, a delay seems to be needed - why?
	try {
	    Thread.sleep(2000);
	} catch (Exception e) {
	    System.out.println("sleep failed");
	}
	return true;
    }

    public boolean respondToHeader() throws IOException {
	if (!ok) { return false; }
	// fake response for now
	byte b[] = { 'Y', 'A', 0, 0, 0, 0, 'R', 'P' };
	out.write(b);
	out.flush();
	return true;
    }

    public boolean sendIndex() throws IOException {
	int len = content.size();
	out.write(new byte[] { 'Y', 'A', 10, 0, 0, 0, 'R', 'P' });
	out.write(new byte[] {(byte)len, 1,
			      -128,-128,-128,-128,  
			      -128,-128,-128,-128});
	byte b0[] = {};
	for (int i=0; i<len; i++) {
	    out.write(netInt(((byte[])content.get(i)).length));
	}
	out.write(netInt(0)); // reply length
	return true;
    }

    public boolean sendContent() throws IOException {
	int len = content.size();
	for (int i=0; i<len; i++) {
	    out.write(((byte[])content.get(i)));
	}
	return true;
    }

    public boolean expectIndex() throws IOException {
	if (!ok) { return false; }
	int len = 0;
	{
	    // expect index header
	    
	    byte b[] = new byte[8];
	    readFull(b);
	    if (b[0] != (byte)'Y') {
		System.out.println("Garbage message");
		for (int i=0; i<b.length; i++) {
		    System.out.println(">>> " + i + " " + (int)b[i] + " " + (char)b[i]);		    
		}
		ok = false;
		return false;
	    }
	    len = netInt(new byte[] {b[2], b[3], b[4], b[5]});
	    if (len!=10) {
		System.out.println("Strange message");
		ok = false;
		return false;
	    }
	}

	int in_len = 0, out_len = 0;
	{
	    // expect index
	    byte b[] = new byte[len];
	    readFull(b);
	    in_len = b[0];
	    out_len = b[1];
	}
	
	len = 0;
	for (int i=0; i<in_len; i++) {
	    byte b[] = new byte[4];
	    readFull(b);
	    len += netInt(b);
	    System.out.println("In len: " + netInt(b));
	    inputLen.add(new Integer(netInt(b)));
	}

	for (int i=0; i<out_len; i++) {
	    byte b[] = new byte[4];
	    readFull(b);
	    System.out.println("Out len: " + netInt(b));
	    outputLen.add(new Integer (netInt(b)));
	}
	System.out.println("Total message length: " + len);
	messageLen = len;
	return true;
    }

    public boolean respondToIndex() throws IOException {
	if (!ok) { return false; }
	return true;
    }


    public boolean expectBlock(byte[] b, int len) throws IOException {
	if (len==0) { return ok; }
	if (!ok) { return false; }
	if (len<0) { len = messageLen; }
	if (messageLen>=len && len>0) {
	    readFull(b);
	    //System.out.println("Got bytes " + b.length);
	    messageLen -= len;
	    return true;
	}
	ok = false;
	return ok;
    }

    public byte[] expectBlock(int len) throws IOException {
	byte b[] = new byte[len];
	boolean result = expectBlock(b,len);
	return result?b:null;
    }

    public boolean respondToBlock() throws IOException {
	if (!ok) { return false; }
	return true;
    }

    public int getSize() {
	if (!ok) { return 0; }
	return messageLen;
    }

    public boolean isOk() {
	return ok;
    }


    public int expectInt() throws IOException {
	byte b[] = new byte[4];
	readFull(b);
	messageLen -= 4;
	return netInt(b);    
    }

    public void addContent(byte[] data) {
	content.add(data);
    }

    public void beginContent() {
	content.clear();
    }

    public void endContent() {
    }
}

