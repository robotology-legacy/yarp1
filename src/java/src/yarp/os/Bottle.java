
package yarp.os;

import java.io.*;
import java.util.*;

class Bottle {
    private List items = new ArrayList();

    public void add(Object o) {
	items.add(o);
    }

    public void clear() {
	items.clear();
    }

    public String toString() {
	return items.toString();
    }

    public List asList() {
	return Collections.unmodifiableList(items);
    }

    public void set(byte[] data) {
	clear();
	int index = 0;
	int len = data.length;
	while (index<len-4) {
	    int id = NetType.netInt(data,index,4);
	    index+=4;
	    System.out.println("Id is " + id);
	    switch (id) {
	    case 1:
		int v = NetType.netInt(data,index,4);
		index+=4;
		System.out.println(" > num is " + v);
		add(new Integer(v));
		break;
	    case 5:
		int l = NetType.netInt(data,index,4);
		index+=4;
		byte[] sub = new byte[l];
		System.arraycopy(data,index,sub,0,l);
		index+=l;
		System.out.println(" > string is " + new String(sub));
		add(new String(sub));
		break;
	    }
	}
    }
}

