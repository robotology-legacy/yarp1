
package yarp.os;

import java.util.*;

public class ContentPool {
    private ContentCreator creator;
    private List in = new LinkedList();
    //private Set out = new HashSet();

    public ContentPool(ContentCreator creator) {
	this.creator = creator;
    }

    public synchronized Content create() {
	Content next = creator.create();
	in.add(next);
	return next;
    }

    public synchronized Content get() {
	if (in.size()==0) {
	    create();
	}
	Iterator it = in.iterator();
	Content next = (Content)it.next();
	in.remove(next);
	//out.add(next);
	return next;
    }

    public synchronized void unget(Content content) {
	//out.remove(content);
	in.add(content);
    }
}

