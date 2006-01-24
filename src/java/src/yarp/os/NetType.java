
package yarp.os;

import java.io.*;
import java.util.*;

class NetType {

    public static int netInt(byte[] b) {
	return netInt(b,0,-1);
    }

    public static int netInt(byte[] b, int offset, int len) {
	long x = 0;
	if (len<0) { len = b.length-offset; }
	for (int i=len-1; i>=0; i--) {
	    //System.out.println("int " + i + " is " + unsigned(b[i+offset]));
	    x *= 256;
	    x += unsigned(b[i+offset]);
	}
	if (x>Integer.MAX_VALUE) { x -= 2*((long)Integer.MIN_VALUE); }
	return (int)x;
    }

    public static byte[] netInt(int x) {
	long x2 = x;
	if (x2<0) { x2 -= 2*((long)Integer.MIN_VALUE); }

	byte b[] = new byte[4];
	for (int i=0; i<4; i++) {
	    long bi = x2%256;
	    b[i] = (byte)bi;
	    x2 /= 256;
	}
	return b;
    }

    public static byte[] netString(String s) {
	byte b[] = new byte[s.length()+1];
	for (int i=0; i<s.length(); i++) {
	    b[i] = (byte)s.charAt(i);
	}
	b[s.length()] = '\0';
	return b;
    }

    public static int unsigned(byte b) {
	int v = b;
	if (v<0) { v = 256+v; }
	return v;
    }

    public static String netString(byte[] txt) {
	String s = new String(txt);
	if (s.length()>=1) {
	    int tlen = s.length();
	    if (s.charAt(tlen-1)=='\0') {
		s = s.substring(0,tlen-1);
	    }
	}
	//Logger.get().println("netstring is [" + s + "] (" + s.length() + " chars)");
	return s;
    }

    public static byte[] append(byte[] b1, byte[] b2) {
	byte[] b3 = new byte[b1.length+b2.length];

	System.arraycopy (b1, 0, b3, 0, b1.length);
	System.arraycopy (b2, 0, b3, b1.length,
			  b2.length); 
	return b3;
    }

    public static String addStrings(String[] strs) {
	return addStrings(Arrays.asList(strs));
    }

    public static String addStrings(List lst) {
	StringBuffer buf = new StringBuffer("");
	boolean first = true;
	for (Iterator it = lst.iterator(); it.hasNext(); ) {
	    Object o = it.next();
	    if (!first) {
		buf.append(" ");
	    }
	    buf.append(o.toString());
	    first = false;
	}
	return buf.toString();
	
    }

    public static String readLine(InputStream is) throws IOException {
	StringBuffer buf = new StringBuffer("");
	boolean done = false;
	while (!done) {
	    int v = is.read();
	    char ch = (char)v;
	    if (ch>=32) {
		buf.append(ch);
	    }
	    if (ch=='\n') {
		done = true;
	    }
	    if (v<0) { 
		throw new IOException();
	    }
	}
	return buf.toString();
    }    
}

