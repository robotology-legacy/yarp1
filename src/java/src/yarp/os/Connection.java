
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
    private static Logger log = Logger.get();

    public Connection(Address address, 
		      String fromKey, 
		      String toKey) throws IOException {
	this.address = address;
	this.fromKey = fromKey;
	this.toKey = toKey;

	try {
	    socket = new Socket(address.getName(),address.getPort());
	} catch (UnknownHostException e) {
	    throw(new IOException("Don't know about host " + address.getName()));
	} catch (IOException e) {
	    //log.error("Couldn't make a connection to " + address);
	    throw(new IOException("Couldn't make a connection to " + address));
	}
	    
	if (socket==null) {
	    System.exit(1);
	}
	    
	try {
	    
	    proto = new Protocol(new TcpCarrier(socket));
	    //proto = new Protocol(new CarrierShiftStream(socket));
	    
	    proto.setSender(fromKey);
	    proto.setRawProtocol(address.getCarrier());
	    proto.sendHeader();
	    proto.expectReplyToHeader();
	    log.println("Connection created ok");
	} catch (IOException e) {
	    log.warning("Problem creating connection to " + address);
	}
    }

    public String getFromName() {
	return fromKey;
    }

    public String getToName() {
    	return toKey;
    }

    public void close() {
	boolean needExternal = false;
	try {
	    needExternal = proto.isConnectionless();
	} catch (IOException e) {
	    log.warning("problem closing connection");
	}
	log.println("** closing connection");
	proto.close();
	log.println("** connection closed");
	if (needExternal) {
	    // probably not interoperable with C++ YARP, need to look
	    // up what the right command is
	    log.println("Attempting to ask " + getToName() +
			" to disconnect from " + getFromName());
	    try {
		YarpClient.command(getToName(),"~" +getFromName());
	    } catch (IOException e) {
		// this is fine, other side closed down first
	    }
	}
    }

    /*
    public void write(String msg) throws IOException {
	write(msg,false);
    }


    // specialized for Bottle
    // not used any more, was just to understand things better
    public void write(String msg, boolean eof) throws IOException {

	InputStream in = null;
        OutputStream out = null;
	assert(socket!=null);
	in = socket.getInputStream();
	out = socket.getOutputStream();
	assert(in!=null);
	assert(out!=null);
	assert(msg!=null);

	out.write(new byte[] { 'Y', 'A', 10, 0, 0, 0, 'R', 'P' });
	out.write(new byte[] {2, 1,    
			      -128,-128,-128,-128,  
			      -128,-128,-128,-128});

	byte b2[] = {1,0,0,0, (byte)(eof?1:0),0,0,0};        // INT 0
	b2 = NetType.append(b2, new byte[] {5,0,0,0}); // STR
	b2 = NetType.append(b2,NetType.netInt(msg.length()+1));
	b2 = NetType.append(b2,NetType.netString(msg));

	byte b1[] = { 0,0,0,0,  '~', 'd', 0, 1};
	b1 = NetType.append(b1,new byte[] { 4,0,0,0 });
	b1 = NetType.append(b1,new byte[] { 'n','/','a',0 });
	b1 = NetType.append(b1,NetType.netInt(b2.length));

	byte b0[] = {};
	b0 = NetType.append(b0,NetType.netInt(b1.length)); // block 1 length
	b0 = NetType.append(b0,NetType.netInt(b2.length)); // block 2 length
	b0 = NetType.append(b0,NetType.netInt(0));  // response length
    

	out.write(b0);
	out.write(b1);
	out.write(b2);
	out.flush();
    }
    */

    /*
    public void writeCommand(char ch, String str) throws IOException {

	proto.beginContent();

	byte[] hdr = NetType.append(NetType.netInt(str.length()+1),
				    new byte[] { '~', (byte)ch, 0, 1});
	proto.appendBlock(hdr); // data hdr
	//proto.appendBlock(new byte[] { 0,0,0,0, '~', (byte)ch, 0, 1}); // data hdr
	if (str!=null) {
	    proto.appendString(str);
	}

	proto.endContent();
	proto.sendIndex();
	proto.sendContent();
	proto.expectAck();
    }
    */

    public void write(Content content) throws IOException {
	write(content,true);
    }

    public void write(Content content, boolean tagAsData) throws IOException {
	
	assert(content!=null);

	if (proto.isActive()) {
	    proto.beginContent();
	    if (tagAsData) {
		// data header
		proto.addContent(new byte[] { 0,0,0,0,  '~', 'd', 0, 1}); 
	    }
	    content.write(proto);
	    proto.endContent();
	    proto.sendIndex();
	    proto.sendContent();
	    proto.expectAck();
	} else {
	    log.println("Skipping explicit output from " + fromKey + 
			" to " + toKey);
	}
    }

    public String getCarrierName() throws IOException {
	return proto.getCarrierName();
    }

    /*
    public static void show(String from, String msg) {
	boolean admin = true;
	if (from.length()>0) {
	  if (from.charAt(0) == '/') {
	      admin = false;
	  }
	}
	if (admin) {
	    log.println(msg);
	} else {
	    log.info(msg);
	}
    }
    */


}

