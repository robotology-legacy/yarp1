
package yarp.os;

import java.io.*;
import java.net.*;
import java.nio.channels.*;
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
    private String carrier = "tcp";
    private Address mcastAddress;

    public Protocol(ShiftStream shift) throws IOException {
	this.shift = shift;
	this.in = shift.getInputStream();
	this.out = shift.getOutputStream();
    }

    public void setSender(String name) {
	senderName = name;
    }

    public void setRawProtocol(String carrier) {
	this.carrier = carrier;
	if (carrier.equals("mcast")) {
	    System.out.println("Getting mcast info");
	    mcastAddress = NameClient.getNameClient().mcastQuery(senderName);
	    System.out.println("mcast address is " + mcastAddress);
	}
    }

    public static int unsigned(byte b) {
	int v = b;
	if (v<0) { v = 256+v; }
	return b;
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
	    x += unsigned(b[i]);
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
	byte p = 0x64;
	if (carrier.equals("udp")) {
	    p = 97;
	}
	if (carrier.equals("mcast")) {
	    p = 98;
	    System.out.println("set protocol specifier to mcast");
	}
	b[2] = p;
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
	    System.out.println("Garbage message at protocol specifier level");
	    return false;
	}

	System.out.println("protocol number is " + b[2]);
	switch(b[2]) {
	case 100:
	    System.out.println("stay with TCP");
	    carrier = "tcp";
	    break;
	case 97:
	    System.out.println("should switch to UDP");
	    carrier = "udp";
	    break;
	case 98:
	    //System.out.println("this protocol observed during UDP disconnect");
	    //System.out.println("not currently handled");
	    System.out.println("should switch to MCAST");
	    carrier = "mcast";
	    break;
	default:
	    System.out.println("unknown protocol");
	    break;
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
	if (carrier.equals("mcast")) {
	    System.out.println("Sending mcast info");
	    byte b[] = new byte[6];

	    InetAddress inet = InetAddress.getByName(mcastAddress.getName());
	    byte[] raw = inet.getAddress();
	    if (raw.length!=4) {
		System.err.println("address strange size");
		System.exit(1);
	    }
	    for (int i=0; i<4; i++) {
		b[i] = raw[i];
	    }
	    int port = mcastAddress.getPort();
	    b[5] = (byte)(port%256);
	    b[4] = (byte)(port/256);
	    
	    out.write(b);
	}
	return true;
    }

    public boolean expectHeader() throws IOException {
	ok = true;
	inputLen = new ArrayList();
	outputLen = new ArrayList();
	messageLen = 0;

	if (!expectProtocolSpecifier()) { ok=false; return false; }
	if (!expectSenderSpecifier()) { ok=false; return false; }
	if (carrier.equals("mcast")) {
	    System.out.println("Looking for mcast info");
	    byte b[] = new byte[6];
	    in.read(b);
	    int ip[] = new int[4];
	    for (int i=0; i<4; i++) {
		ip[i] = unsigned(b[i]);
		System.out.println("  >> IP " + ip[i]);
	    }
	    // strange byte order
	    int port = netInt(new byte[] { b[5], b[4], 0, 0 });
	    System.out.println("  >> PORT " + port);

	    mcastAddress = new Address("" + ip[0] + "." + 
				       ip[1] + "." +
				       ip[2] + "." +
				       ip[3],
				       port);
	}
	return true;
    }

    public boolean expectReplyToHeader() throws IOException {
	int port = 0;
	if (!carrier.equals("mcast")) {
	    byte b[] = new byte[8];
	    in.read(b);
	    //System.out.println(b[0]=='Y');
	    port = unsigned(b[2])+256*unsigned(b[3]);
	    System.out.println("Port number is " + port);
	    if (b[0]!='Y') {
		throw new IOException();
	    }
	}
	if (carrier.equals("udp")) {
	    System.out.println("Switching to udp, remote port " + port);
	    shift.becomeUdp(port);
	    this.in = shift.getInputStream();
	    this.out = shift.getOutputStream();
	    System.out.println("Switched");
	}
	if (carrier.equals("mcast")) {
	    System.out.println("Switching to mcast " + mcastAddress);
	    shift.becomeMcast(mcastAddress);
	    this.in = shift.getInputStream();
	    this.out = shift.getOutputStream();
	    System.out.println("Switched");
	}
	// For TCP, a delay seems to be needed - why?
	if (carrier.equals("tcp")) {
	    Time.delay(2);
	}
	return true;
    }

    public boolean respondToHeader() throws IOException {
	if (!ok) { return false; }
	byte b[] = { 'Y', 'A', 0, 0, 0, 0, 'R', 'P' };
	byte b2[] = netInt(shift.getLocalPort());
	for (int i=0; i<b2.length; i++) {
	    b[i+2] = b2[i];
	}
	out.write(b);
	out.flush();

	if (carrier.equals("udp")) {
	    System.out.println("Switching to udp");
	    shift.becomeUdp(-1);
	    this.in = shift.getInputStream();
	    this.out = shift.getOutputStream();
	}
	if (carrier.equals("mcast")) {
	    System.out.println("Should switch to mcast, " + mcastAddress);
	    shift.becomeMcast(mcastAddress);
	    this.in = shift.getInputStream();
	    this.out = shift.getOutputStream();
	}

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
	out.flush();
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
		System.out.println("Garbage message at index level");
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

