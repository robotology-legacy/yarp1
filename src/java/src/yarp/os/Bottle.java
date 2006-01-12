
package yarp.os;

import java.io.*;
import java.util.*;

class Bottle {
    private static Logger log = Logger.get();

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

    public byte[] get() {
	byte[] b = {};
	for (Iterator it = items.iterator(); it.hasNext(); ) {
	    Object o = it.next();
	    if (o instanceof Integer) {
		b = append(b,NetType.netInt(1));
		b = append(b,NetType.netInt(((Integer)o).intValue()));
	    } else if (o instanceof String) {
		b = append(b,NetType.netInt(5));
		b = append(b,NetType.netInt(((String)o).length()+1));
		b = append(b,NetType.netString((String)o));
	    }
	}
	return b;
    }

    public void set(byte[] data) {
	clear();
	int index = 0;
	int len = data.length;
	while (index<len-4) {
	    int id = NetType.netInt(data,index,4);
	    index+=4;
	    log.println("Id is " + id);
	    switch (id) {
	    case 1:
		int v = NetType.netInt(data,index,4);
		index+=4;
		log.println(" > num is " + v);
		add(new Integer(v));
		break;
	    case 5:
		int l = NetType.netInt(data,index,4);
		index+=4;
		byte[] sub = new byte[l];
		System.arraycopy(data,index,sub,0,l);
		index+=l;
		String txt = NetType.netString(sub); //new String(sub);
		/*
		if (txt.length()>=1) {
		    int tlen = txt.length();
		    if (txt.charAt(tlen-1)=='\0') {
			txt = txt.substring(0,tlen-1);
		    }
		}
		*/
		log.println(" > string is " + txt);
		add(txt);
		break;
	    }
	}
    }

    private byte[] append(byte[] b1, byte[] b2) {
	byte[] b3 = new byte[b1.length+b2.length];

	System.arraycopy (b1, 0, b3, 0, b1.length);
	System.arraycopy (b2, 0, b3, b1.length,
			  b2.length); 
	return b3;
    }


}

