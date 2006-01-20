
package yarp.os;

import java.io.IOException;


class BackgroundWriter extends Thread implements ContentWriter {
    private static Logger log = Logger.get();
    private Protocol proto;
    private Object writeSomething = new Object();
    private Object stateMutex = new Object();
    private boolean writing = false;
    private boolean finished = false;
    private ContentWriter delegate;

    private Content content = null;
    private boolean tagAsData = false;

    public BackgroundWriter(Protocol proto) {
	this.proto = proto;
	delegate = new SlowWriter(proto);
	start();
    }

    public void write(Content content, boolean tagAsData) throws IOException {
	synchronized(stateMutex) {
	    if (!writing) {
		if (this.content!=null) {
		    log.println("Dropping a message for a slow reader");
		}
		this.content = content;
		this.tagAsData = tagAsData;
		synchronized(writeSomething) {
		    writeSomething.notify();
		}
	    } 
	}
    }

    public void close() throws IOException {
	finished = true;
	synchronized(writeSomething) {
	    writeSomething.notify();
	}
    }

    public void run() {
	while (!finished) {
	    try {
		synchronized(writeSomething) {
		    writeSomething.wait();
		}
		synchronized(stateMutex) {
		    writing = true;
		}
		if (!finished) {
		    try {
			//for (int i=0; i<3; i++) {
			//  log.info("simulating writing something, slowly");
			//  Time.delay(1);
			//}
			delegate.write(content,tagAsData);
		    } catch (IOException e) {
			log.error("connection failed: " + e);
		    }
		}
		synchronized(stateMutex) {
		    writing = false;
		    content = null;
		}
	    } catch (InterruptedException e) {
		log.error("interrupted connection: " + e);
	    }
	}
    }
}
