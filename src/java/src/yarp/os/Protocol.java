
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
    private Logger log = Logger.get();

    private ArrayList inputLen = new ArrayList();
    private ArrayList outputLen = new ArrayList();
    private int messageLen = 0;
    private boolean ok = true;
    private boolean requireAck = false;
    private boolean pendingAck = false;

    private String senderName = "null";
    private ArrayList content = new ArrayList();
    private String carrier = "tcp";
    private Carrier delegate;

    public Protocol(ShiftStream shift) throws IOException {
	log.println("Protocol manager object starting with address " +
		    shift.getAddress());
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
	if (delegate!=null) {
	    if (name.equals(delegate.getName())) {
		return delegate;
	    }
	}
	Carrier c = shift.chooseCarrier(name);
	//c.setAddress(shift.getAddress(),null);
	return c;
    }

    public Carrier chooseCarrier(int specifier) {
	if (delegate!=null) {
	    if (delegate.getSpecifier()==specifier) {
		return delegate;
	    }
	}
	Carrier c = shift.chooseCarrier(specifier);
	return c;
    }

    public Carrier chooseCarrier(byte[] header) {
	if (delegate!=null) {
	    if (delegate.alternateHeaderCheck(header)) {
		return delegate;
	    }
	}
	Carrier c = shift.chooseCarrier(header);
	return c;
    }

    public void checkForNewStreams() throws IOException {
	InputStream i = shift.getInputStream();
	OutputStream o = shift.getOutputStream();
	if (i!=null) {
	    in = i;
	}
	if (o!=null) {
	    out = o;
	}
    }


    public void become(String carrier, Address address) throws IOException {
	log.println("Switching to " + carrier);
	shift.setIncoming(delegate);
	shift.become(carrier,address);
	checkForNewStreams();
	//this.in = shift.getInputStream();
	//this.out = shift.getOutputStream();
    }

    public String getCarrierName() {
	if (delegate!=null) { 
	    return delegate.getName();
	}
	return carrier;
    }

    public void setRawProtocol(String carrier) throws IOException {
	if (carrier==null) {
	    carrier = new Address("127.0.0.1",1).getCarrier(); // find default
	}
	this.carrier = carrier;
	delegate = chooseCarrier(carrier);
	assert(delegate!=null);
	checkForNewStreams();
	//this.in = shift.getInputStream();
	//this.out = shift.getOutputStream();
	if (!delegate.isConnectionless()) {
	    // new YARP2 policy
	    requireAck = true;
	}
	delegate.prepareSend(this);
    }

    public int readFull(byte[] b) throws IOException {
	int off = 0;
	int fullLen = b.length;
	int remLen = fullLen;
	int result = 1;
	while (result>0&&remLen>0) {
	    result = in.read(b,off,remLen);
	    if (result>0) {
		remLen -= result;
		off += result;
	    }
	}
	return (result<0)?result:fullLen;
    }

    private boolean sendProtocolSpecifier() throws IOException {
	byte b[] = { 'Y', 'A', 0x64, 0x1e, 0, 0, 'R', 'P' };
	
	//byte p = 0x64;
	//p = (byte)delegate.getSpecifier();
	//b[2] = p;

	int p = delegate.getSpecifier();
	p += 7777;
	if (requireAck) { p += 128; }

	byte b2[] = NetType.netInt(p);
	for (int i=0; i<b2.length; i++) {
	    b[i+2] = b2[i];
	}
	write(b);
	out.flush();
	return true;
    }


    private boolean expectProtocolSpecifier() throws IOException {
	// expect protocol specifying header.

	byte b[] = new byte[8];
	readFull(b);
	String chk1 = NetType.netString(new byte[] { b[0], b[1], b[6], b[7] });
	String chk2 = NetType.netString(b);

	int specifier = -1;
	if (chk1.equals("YARP")) {
	    specifier = NetType.unsigned(b[2]) + NetType.unsigned(b[3])*256;
	    specifier -= 7777; // magic number, who knows why
	    requireAck = (specifier&128)!=0;
	    specifier %= 16;
	    log.println("specifier is " + specifier + " with ack " + requireAck);
	    delegate = chooseCarrier(specifier);
	} else {
	    log.println("choosing by alternate header");
	    delegate = chooseCarrier(b);
	}

	if (delegate==null) {
	    log.error("unknown protocol with code " + specifier);
	    if (specifier==2) {
		log.error("In fact that is SHMEM, but that is not supported by Java");
	    }
	    System.exit(1);
	}
	//this.in = shift.getInputStream();
	//this.out = shift.getOutputStream();
	checkForNewStreams();


	gotHeader = true;
	// should do something with it!

	return true;
    }

    private boolean sendSenderSpecifier() throws IOException {
	write(NetType.netInt(senderName.length()+1));
	write(NetType.netString(senderName));
	out.flush();
	return true;
    }

    // part of header
    private boolean expectSenderSpecifier() throws IOException {
	delegate.expectSenderSpecifier(this);
	log.println("name is " + senderName);

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
	return delegate.respondToHeader(this);
    }

    public boolean defaultRespondToHeader() throws IOException {
	if (!ok) { return false; }
	byte b[] = { 'Y', 'A', 0, 0, 0, 0, 'R', 'P' };
	assert(shift.getAddress()!=null);
	int cport = shift.getAddress().getPort();
	log.println("setting port number to " + cport);
	byte b2[] = NetType.netInt(cport);
	for (int i=0; i<b2.length; i++) {
	    b[i+2] = b2[i];
	}

	write(b);
	out.flush();

	delegate.respondExtraToHeader(this);

	return true;
    }

    public boolean sendIndex() throws IOException {
	int len = content.size();
	write(new byte[] { 'Y', 'A', 10, 0, 0, 0, 'R', 'P' });
	write(new byte[] {(byte)len, 1,
			  -1,-1,-1,-1,  
			  -1,-1,-1,-1});
	//-128,-128,-128,-128,  
	//	      -128,-128,-128,-128});
	byte b0[] = {};
	for (int i=0; i<len; i++) {
	    write(NetType.netInt(((byte[])content.get(i)).length));
	}
	write(NetType.netInt(0)); // reply length
	return true;
    }

    public boolean sendContent() throws IOException {
	int len = content.size();
	for (int i=0; i<len; i++) {
	    byte[] data = (byte[])content.get(i);
	    write(data);
	}
	out.flush();
	return true;
    }

    public boolean expectIndex() throws IOException {
	pendingAck = true;
	messageLen = 0;
	if (!ok) { return false; }
	return delegate.expectIndex(this);
    }

    public boolean defaultExpectIndex() throws IOException {
	if (!ok) { return false; }
	int len = 0;
	{
	    // expect index header
	    
	    byte b[] = new byte[8];
	    int r = readFull(b);
	    if (r<=0) {
		throw new IOException();
	    }
	    if (b[0] != (byte)'Y') {
		log.println("Garbage message at index level");
		for (int i=0; i<b.length; i++) {
		    System.out.println(">>> " + i + " " + (int)b[i] + " " + (char)b[i]);		    
		}
		ok = false;
		return false;
	    }
	    len = NetType.netInt(new byte[] {b[2], b[3], b[4], b[5]});
	    if (len!=10) {
		log.warning("Strange message");
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
	    log.println("In len: " + NetType.netInt(b));
	    inputLen.add(new Integer(NetType.netInt(b)));
	}

	for (int i=0; i<out_len; i++) {
	    byte b[] = new byte[4];
	    readFull(b);
	    log.println("Out len: " + NetType.netInt(b));
	    outputLen.add(new Integer (NetType.netInt(b)));
	}
	log.println("Total message length: " + len);
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
	return NetType.netString(expectBlock(len));
    }

    public String expectLine() throws IOException {
	return NetType.readLine(in);
    }

    public boolean respondToBlock() throws IOException {
	byte b[] = new byte[100];
	write(b);
	if (!ok) { return false; }
	return true;
    }

    public boolean expectAck() throws IOException {
	//log.info("expectAck - " + requireAck());
	if (!ok) { return false; }
	if (requireAck()) {
	    //log.info("Ok I'm going to see if there is a reply");
	    byte b[] = new byte[8];
	    readFull(b);
	    String chk1 = 
		NetType.netString(new byte[] { b[0], b[1], b[6], b[7] });
	    log.println("expectAck check is " + chk1);
	    if (chk1.equals("YARP")) {
		int len = 
		    NetType.netInt(new byte[] { b[2], b[3], b[4], b[5] });
		//log.info("len is " + len);
		if (len>0) {
		    log.println("expecting ack len " + len);
		    byte b2[] = new byte[len];
		    readFull(b2);
		    log.println("done with expecting ack len " + len);
		}
	    } else {
		log.error("Failed");
	    }
				  
	}

	return true;
    }

    public boolean requireAck() {
	return requireAck;
    }

    public boolean sendAck() throws IOException {
	pendingAck = false;
	if (requireAck()) {
	    return delegate.sendAck(this);
	}
	return true;
    }

    public boolean defaultSendAck() throws IOException {
	if (requireAck()) {
	    log.println("sending ack");
	    byte b[] = { 'Y', 'A', 0, 0, 0, 0, 'R', 'P' };
	    write(b);
	    out.flush();
	}
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

    public void appendLine(String data) {
	appendString(data + "\n");
    }

    public void addContent(byte[] data) {
	content.add(data);
    }

    public void beginContent() {
	content.clear();
    }

    public void endContent() {
	log.println("raw protocol is " + carrier);
    }

    public void close() {
	try {
	    if (pendingAck) {
		try {
		    sendAck();
		} catch (IOException e) {
		    // don't worry about this, just make best effort
		}
	    }
	    shift.close();
	} catch (IOException e) {
	    log.error("Problem closing protocol handler");
	}
    }

    public void write(byte[] b) throws IOException {
	//log.println(">>>> Sending block of length " + b.length + " content " +
	//    NetType.netString(b));

	out.write(b);
    }

    public void read(byte[] b) throws IOException {
	in.read(b);
    }

    public String readLine() throws IOException {
	return expectLine();
    }

    public boolean isActive() throws IOException {
	if (delegate!=null) {
	    return delegate.isActive();
	}
	return true;
    }

    public boolean isConnectionless() throws IOException {
	if (delegate!=null) {
	    return delegate.isConnectionless();
	}
	return true;
    }

    public OutputStream getOutputStream() throws IOException {
	return shift.getOutputStream();
    }

    public boolean isTextMode() {
	if (delegate!=null) {
	    return delegate.isTextMode();
	}
	return false;
    }
}

