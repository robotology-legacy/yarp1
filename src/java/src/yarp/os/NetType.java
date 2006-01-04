
package yarp.os;

public class NetType {

    public static int netInt(byte[] b) {
	return netInt(b,0,-1);
    }

    public static int netInt(byte[] b, int offset, int len) {
	int x = 0;
	if (len<0) { len = b.length-offset; }
	for (int i=len-1; i>=0; i--) {
	    x *= 256;
	    x += (int)b[i+offset]; // warning, may have sign trouble
	}
	return x;
    }

    public static byte[] netInt(int x) {
	byte b[] = new byte[4];
	for (int i=0; i<4; i++) {
	    int bi = x%256;
	    b[i] = (byte)bi;
	    x /= 256;
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
	return b;
    }


}

