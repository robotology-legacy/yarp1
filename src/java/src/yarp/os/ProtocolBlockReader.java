
package yarp.os;

import java.io.*;

class ProtocolBlockReader implements BlockReader {
    private static Logger log = Logger.get();

    private InputStream in;
    private int messageLen = 0;
    private boolean textMode = false;

    public ProtocolBlockReader() {
    }

    public void set(InputStream in, int len, boolean textMode) {
	this.in = in;
	this.messageLen = len;
	this.textMode = textMode;
    }

    public boolean expectBlock(byte[] b, int len) throws IOException {
	if (len==0) { return true; }
	if (len<0) { len = messageLen; }
	if (messageLen>=len && len>0) {
	    readFull(b);
	    messageLen -= len;
	    return true;
	}
	return false;
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

    public int expectInt() throws IOException {
	byte b[] = new byte[4];
	readFull(b);
	messageLen -= 4;
	return NetType.netInt(b);    
    }

    public boolean isTextMode() throws IOException {
	return textMode;
    }


    public int getSize() throws IOException {
	return messageLen;
    }

    private int readFull(byte[] b) throws IOException {
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



}

