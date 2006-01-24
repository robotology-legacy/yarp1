
package yarp.os;

import java.util.*;
import java.io.*;


/**
 * A flexible data format for holding a bunch of numbers and strings.
 * Handy to use until you work out how to make your own more 
 * efficient formats for transmission.
 */
public class Bottle {
    private static Logger log = Logger.get();

    private List items = new ArrayList();

    /**
     * Add an object to the bottle.  Currently, integers and strings are
     * supported for transmission.
     */
    public void add(Object o) {
	items.add(o);
    }

    /**
     * Clear the contents of the bottle.
     */
    public void clear() {
	items.clear();
    }

    /**
     * Set the contents of the bottle by parsing a text string.
     */
    public void fromString(String line) {
	clear();
	try {
	    StreamTokenizer tok = 
		new StreamTokenizer(new StringReader(line));
	    while (true) {
		int token = tok.nextToken();
		if (token == StreamTokenizer.TT_EOF) {
		    break;
		}
		else if (token == StreamTokenizer.TT_NUMBER) {
		    add(new Integer((int)tok.nval));
		} else {
		    add(new String(tok.sval));
		}
	    }
	} catch (Exception e) {
	    Logger.get().error("Strange bottle format");
	}
    }

    /**
     * Express the contents of the bottle as a string.
     */
    public String toString() {
	StringBuffer buf = new StringBuffer("");
	boolean add = false;
	for (Iterator it = items.iterator(); it.hasNext(); ) {
	    Object o = it.next();
	    String txt = o.toString();
	    if (o instanceof String) {
		StringBuffer quoted = new StringBuffer("");
		for (int i=0; i<txt.length(); i++) {
		    char ch = txt.charAt(i);
		    switch (ch) {
		    case '\\':
			quoted.append(ch);
			quoted.append(ch);
			break;
		    case '\"':
			quoted.append('\\');
			quoted.append(ch);
			break;
		    default:
			quoted.append(ch);
			break;
		    }
		}
		
		txt = "\"" + quoted + "\"";
	    }
	    if (add) {
		buf.append(" ");
	    }
	    buf.append(txt);
	    add = true;
	}
	return buf.toString();
    }

    /**
     * View the contents of the bottle as a list.
     */
    public List asList() {
	return Collections.unmodifiableList(items);
    }

    public int size() {
	return items.size();
    }

    /**
     * View the contents of the bottle as an array of bytes.
     */
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

    /**
     * Set the contents of the buffer from an array of bytes.
     */
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
		String txt = NetType.netString(sub);
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

