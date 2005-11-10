
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
}

