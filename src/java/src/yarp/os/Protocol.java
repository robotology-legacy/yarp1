
package yarp.os;

import java.io.*;
import java.util.*;

/**
 * Internal communication manager.
 * A Protocol object manages a bunch of ways of viewing a single stream
 */
class Protocol implements InputProtocol, OutputProtocol {
    private ShiftStream shift;
    private InputStream in;
    private OutputStream out;

    private boolean didHeader = false;
    private boolean isReader = true;
    private boolean isWriter = true;
    private boolean needIndex = true;

    private Logger log = Logger.get();

    private ArrayList inputLen = new ArrayList();
    private ArrayList outputLen = new ArrayList();
    private boolean ok = true;
    private boolean pendingAck = false;

    private ArrayList content = new ArrayList();
    private Carrier delegate;

    private ProtocolBlockReader reader = new ProtocolBlockReader();
    private ProtocolBlockWriter writer = new ProtocolBlockWriter();
    private int messageLen = 0;

    private Route route = new Route("null","null","tcp");

    public Protocol(ShiftStream shift) throws IOException {
	log.println("Protocol starting with address " +
		    shift.getLocalAddress());
	this.shift = shift;
	this.in = shift.getInputStream();
	this.out = shift.getOutputStream();
    }

    public Route getRoute() {
	return route;
    }

    public void setRoute(Route route) {
	this.route = route;
	log = new Logger(route.toString(),Logger.get());
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


    public void become(Carrier next, Address address) throws IOException {
	log.println("Switching stream to carrier: " + next.getName());

	next.open(address,shift);
	//shift.close();
	delegate = next;
	setRoute(getRoute().addCarrierName(delegate.getName()));
	shift = next;

	checkForNewStreams();
    }


    public void setRawProtocol(String carrier) throws IOException {
	log.println("setting carrier to " + carrier);
	if (carrier==null) {
	    carrier = new Address("127.0.0.1",1).getCarrier(); // find default
	}
	setRoute(getRoute().addCarrierName(carrier));
	log.assertion(delegate == null);
	delegate = Carriers.chooseCarrier(carrier);
	log.assertion(delegate!=null);
	checkForNewStreams();
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
	    log.println("result " + result);
	}
	return (result<0)?result:fullLen;
    }

    private boolean sendProtocolSpecifier() throws IOException {
	write(delegate.getHeader());
	out.flush();
	return true;
    }


    private boolean expectProtocolSpecifier() throws IOException {
	// expect protocol specifying header.

	byte b[] = new byte[8];
	readFull(b);
	String chk1 = NetType.netString(new byte[] { b[0], b[1], b[6], b[7] });

	boolean already = false;
	if (delegate!=null) {
	    if (delegate.checkHeader(b)) {
		already = true;
	    }
	}
	if (!already) {
	    delegate = Carriers.chooseCarrier(b);
	}

	if (delegate==null) {
	    log.error("unrecognized protocol");
	    System.exit(1);
	}
	delegate.setParameters(b);
	checkForNewStreams();

	return true;
    }

    private boolean sendSenderSpecifier() throws IOException {
	String senderName = getRoute().getFromName();
	write(NetType.netInt(senderName.length()+1));
	write(NetType.netString(senderName));
	out.flush();
	return true;
    }

    // part of header
    private boolean expectSenderSpecifier() throws IOException {
	delegate.expectSenderSpecifier(this);
	log.println("name is " + getRoute().getFromName());
	return true;
    }

    public boolean defaultExpectSenderSpecifier() throws IOException {
	//log.println("expectSenderSpecifer for " + getName());
	int len = 0;
	// expect an ID string length -- an integer.
	byte bLen[] = new byte[4];
	readFull(bLen);
	len = NetType.netInt(bLen);
	if (len>1000) len = 1000;
	if (len<1) len = 1;
	// expect a null-terminated string
	byte b[] = new byte[len];
	readFull(b);
	setRoute(getRoute().addFromName(NetType.netString(b)));
	return true;
    }

    public boolean sendHeader() throws IOException {
	return delegate.sendHeader(this);
    }

    public boolean defaultSendHeader() throws IOException {
	sendProtocolSpecifier();
	sendSenderSpecifier();
	//delegate.sendExtraHeader(this);

	return true;
    }

    public boolean expectHeader() throws IOException {
	// cannot delegate this since delegate doesn't get chosen
	// until header is read

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
	writer.set(content,isTextMode());

	return true;
    }

    public boolean respondToHeader() throws IOException {
	boolean result = delegate.respondToHeader(this);
	out.flush();
	return result;
    }


    public int readYarpInt() throws IOException {
	int port = 0;
	byte b[] = new byte[8];
	read(b);
	//System.out.println(b[0]=='Y');
	port = NetType.unsigned(b[2])+256*NetType.unsigned(b[3]);
	log.println("Port number is " + port);
	if (b[0]!='Y') {
	    throw new IOException();
	}
	return port;
    }


    public void writeYarpInt(int n) throws IOException {
	byte b[] = { 'Y', 'A', 0, 0, 0, 0, 'R', 'P' };
	log.println("writing yarp integer " + n);
	byte b2[] = NetType.netInt(n);
	for (int i=0; i<b2.length; i++) {
	    b[i+2] = b2[i];
	}
	write(b);
    }

    public boolean sendIndex() throws IOException {
	return delegate.sendIndex(this);
    }

    public boolean defaultSendIndex() throws IOException {
	int len = content.size();
	write(new byte[] { 'Y', 'A', 10, 0, 0, 0, 'R', 'P' });
	write(new byte[] {(byte)len, 1,
			  -1,-1,-1,-1,  
			  -1,-1,-1,-1});

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
	boolean result = delegate.expectIndex(this);
	reader.set(getStreams().getInputStream(),messageLen,isTextMode());
	return result;
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

    public boolean respondToBlock() throws IOException {
	byte b[] = new byte[100];
	write(b);
	if (!ok) { return false; }
	return true;
    }

    public boolean expectAck() throws IOException {
	log.println("expectAck - " + requireAck());
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
	return delegate.requireAck();
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

    public int getSize() throws IOException {
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

    public void beginContent() {
	content.clear();
    }

    public void endContent() {
	log.println("raw protocol is " + getRoute().getCarrierName());
    }

    public void close() {
	try {
	    log.println("starting protocol close");
	    if (pendingAck) {
		try {
		    sendAck();
		} catch (IOException e) {
		    // don't worry about this, just make best effort
		    log.println("Problem closing protocol handler");
		}
	    }
	    shift.close();
	    log.println("done protocol close");
	} catch (IOException e) {
	    log.println("Problem closing protocol handler");
	}
    }

    public void write(byte[] b) throws IOException {
	out.write(b);
    }

    public void read(byte[] b) throws IOException {
	in.read(b);
    }

    public String readLine() throws IOException {
	return NetType.readLine(in);
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


    public boolean isTextMode() {
	if (delegate!=null) {
	    return delegate.isTextMode();
	}
	return false;
    }

    public TwoWayStream getStreams() throws IOException {
	return shift;
    }

    public BlockReader getReader() {
	return reader;
    }

    public BlockWriter getWriter() {
	return writer;
    }

    ///////////////////////////////////////////////////////////////////
    // Start to make a cleaner user interface to Protocol object
    // Ideally, the following methods are all that a user of a Protocol
    // object should need.


    public void setTag(boolean isReader, boolean isWriter) {
	this.isReader = isReader;
	this.isWriter = isWriter;
    }

    public void initiate(String name) throws IOException {
	initiate("notset",name,null);
    }

    /**
     * @param opts Name of the desired carrier, e.g. "mcast"
     */
    public void initiate(String from, String to, String opts) 
	throws IOException {

	// can only use these methods on specialized ports
	log.assertion(isReader!=isWriter);
	if (isReader) {
	    if (opts!=null) {
		log.error("Options ignored on reader side");
	    }
	    expectHeader();
	    respondToHeader();
	    setRoute(getRoute().addToName(to));
	} else if (isWriter) {
	    if (opts==null) {
		opts = "text";
	    }
	    setRoute(new Route(from,to,opts));
	    setRawProtocol(opts);
	    sendHeader();
	    expectReplyToHeader();
	} else {
	    log.error("Protocol not specialized to read or write");
	    log.error("Can't decide how to initialize");
	}
	didHeader = true;
	needIndex = true;
    }
    
    public BlockReader beginRead() throws IOException {
	log.assertion(isReader==true);
	log.assertion(didHeader);
	if (needIndex) {
	    expectIndex();
	    respondToIndex();
	}
	return getReader();
    }

    public void endRead() throws IOException {
	sendAck();
    }

    /**
     * May return null if carrier determines this connection is redundant
     */
    public BlockWriter beginWrite() throws IOException {
	log.assertion(isWriter==true);
	log.assertion(didHeader);
	if (isActive()) {
	    beginContent();
	    return getWriter();
	}
	return null;
    }

    public void endWrite() throws IOException {
	endContent();
	sendIndex();
	sendContent();
	expectAck();
    }

    public OutputStream getReplyStream() throws IOException {
	return getStreams().getOutputStream();
    }

    public InputStream getInputStream() throws IOException {
	return getStreams().getInputStream();
    }


    public Address getRemoteAddress() throws IOException {
	return shift.getRemoteAddress();
    }

}

