
package yarp.os;

import java.io.*;
import java.net.*;
import java.nio.channels.*;
import java.util.*;


class Protocol implements BlockWriter, BlockReader {
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
    private Carrier delegate;

    public Protocol(ShiftStream shift) throws IOException {
	this.shift = shift;
	this.in = shift.getInputStream();
	this.out = shift.getOutputStream();
    }

    public void setSender(String name) {
	senderName = NameClient.getNamePart(name);
    }

    public String getSender() {
	return senderName;
    }

    public Carrier chooseCarrier(String name) {
	return CarrierShiftStream.chooseCarrier(name);
    }

    public Carrier chooseCarrier(int specifier) {
	return CarrierShiftStream.chooseCarrier(specifier);
    }

    public void become(String carrier, Address address) throws IOException {
	System.out.println("Switching to " + carrier);
	shift.become(carrier,address);
	this.in = shift.getInputStream();
	this.out = shift.getOutputStream();
    }

    public void setRawProtocol(String carrier) {
	this.carrier = carrier;
	delegate = chooseCarrier(carrier);
	delegate.prepareSend(this);
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

    private boolean sendProtocolSpecifier() throws IOException {
	byte b[] = { 'Y', 'A', 0x64, 0x1e, 0, 0, 'R', 'P' };
	byte p = 0x64;
	p = (byte)delegate.getSpecifier();
	b[2] = p;
	out.write(b);
	out.flush();
	return true;
    }

    // part of header
    private boolean expectProtocolSpecifier() throws IOException {
	// expect protocol specifying header.
	// just guessing the codes - could compute them from MAGIC_NUMBER 
	// stuff

	byte b[] = new byte[8];
	readFull(b);
	if (b[0] != (byte)'Y') {
	    System.out.println("Garbage message at protocol specifier level");
	    return false;
	}

	System.out.println("protocol number is " + b[2]);
	delegate = chooseCarrier((int)b[2]);
	if (delegate==null) {
	    System.err.println("unknown protocol");
	    System.exit(1);
	}

	gotHeader = true;
	// should do something with it!

	return true;
    }

    private boolean sendSenderSpecifier() throws IOException {
	out.write(NetType.netInt(senderName.length()+1));
	out.write(NetType.netString(senderName));
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
	    len = NetType.netInt(b);
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
	delegate.sendExtraHeader(this);

	return true;
    }

    public boolean expectHeader() throws IOException {
	ok = true;
	inputLen = new ArrayList();
	outputLen = new ArrayList();
	messageLen = 0;

	if (!expectProtocolSpecifier()) { ok=false; return false; }
	if (!expectSenderSpecifier()) { ok=false; return false; }
	delegate.expectExtraHeader(this);
	return true;
    }

    public boolean expectReplyToHeader() throws IOException {
	delegate.expectReplyToHeader(this);
	return true;
    }

    public boolean respondToHeader() throws IOException {
	if (!ok) { return false; }
	byte b[] = { 'Y', 'A', 0, 0, 0, 0, 'R', 'P' };
	byte b2[] = NetType.netInt(shift.getAddress().getPort());
	for (int i=0; i<b2.length; i++) {
	    b[i+2] = b2[i];
	}
	out.write(b);
	out.flush();

	delegate.respondExtraToHeader(this);

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
	    out.write(NetType.netInt(((byte[])content.get(i)).length));
	}
	out.write(NetType.netInt(0)); // reply length
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
	    len = NetType.netInt(new byte[] {b[2], b[3], b[4], b[5]});
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
	    len += NetType.netInt(b);
	    System.out.println("In len: " + NetType.netInt(b));
	    inputLen.add(new Integer(NetType.netInt(b)));
	}

	for (int i=0; i<out_len; i++) {
	    byte b[] = new byte[4];
	    readFull(b);
	    System.out.println("Out len: " + NetType.netInt(b));
	    outputLen.add(new Integer (NetType.netInt(b)));
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
	if (len<0) {
	    len = getSize();
	}
	byte b[] = new byte[len];
	boolean result = expectBlock(b,len);
	return result?b:null;
    }

    public String expectString(int len) throws IOException {
	return new String(expectBlock(len));
    }

    public boolean respondToBlock() throws IOException {
	byte b[] = new byte[100];
	out.write(b);
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
	return NetType.netInt(b);    
    }

    public void append(byte[] data) {
	addContent(data);
    }

    public void appendBlock(byte[] data) {
	append(data);
    }

    public void appendInt(int data) {
	append(NetType.netInt(data));
    }

    public void appendString(String data) {
	append(NetType.netString(data));
    }


    public void addContent(byte[] data) {
	content.add(data);
    }

    public void beginContent() {
	content.clear();
    }

    public void endContent() {
	System.out.println("raw protocol is " + carrier);
    }

    public void close() {
	try {
	    shift.close();
	} catch (IOException e) {
	    System.err.println("problem closing shiftsocket");
	}
    }

    public void write(byte[] b) throws IOException {
	out.write(b);
    }

    public void read(byte[] b) throws IOException {
	in.read(b);
    }
}

