
package yarp.os;

import java.io.IOException;

class SlowWriter implements ContentWriter {
    private static Logger log = Logger.get();
    private static CommandContent dataHeader = new CommandContent('d',null);

    private Protocol proto;

    public SlowWriter(Protocol proto) {
	this.proto = proto;
    }

    public void write(Content content, boolean tagAsData) throws IOException {
	
	log.assertion(content!=null);

	if (proto.isActive()) {
	    proto.beginContent();
	    if (tagAsData) {
		dataHeader.write(proto.getWriter());
	    }
	    content.write(proto.getWriter());
	    proto.endContent();
	    proto.sendIndex();
	    proto.sendContent();
	    proto.expectAck();
	} else {
	    log.println("Skipping explicit output from " +
			proto.getRoute().getFromName() + 
			" to " + proto.getRoute().getToName());
	}
    }    

    public void close() throws IOException {
    }
}

