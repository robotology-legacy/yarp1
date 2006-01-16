
package yarp.os;

import java.io.*;

/**
 * A network-visible object for transmitting information.  
 * The port is assigned a name
 * using the register method.  Then it is associated (using the
 * creator method) with a ContentCreator object that is used to
 * make transmittable objects.
 */
public class OutputPort implements Port {

    private ContentCreator creator;
    private Content content;
    private BasicPort port;

    /**
     * Assign a factory for creating transmittable objects.
     * @param creator the object factory
     */
    public void creator(ContentCreator creator) {
	this.creator = creator;
    }

    /**
     * Assign a unique global name to this port.
     * @param name The name to assign.
     */
    public void register(String name) {
	BasicPort basic = new BasicPort(name);
	port = basic;
	basic.start();
    }

    /**
     * Request the port to send output to an input port.
     * @param src The name of the input port.
     */
    public void connect(String src) {
	try {
	    port.connect(src);
	} catch (IOException e) {
	    Logger.get().error("Could not make connection");
	}
    }

    /**
     * Get a transmittable object.  The creator is called to make this if
     * necessary.
     * @return An object of the type made by the creator.
     */
    public Object content() {
	if (content==null) {
	    content = creator.create();
	}
	return content.object();
    }

    /**
     * Output the transmittable object available from the content method.
     */
    public void write() {
	content();
	port.send(content);
	content = null;
    }

    /**
     * Output an externally managed object.
     * @param content The object to write
     */
    public void write(Content content) {
	port.send(content);
    }

    /**
     * Shut the port down.
     */
    public void close() {
	port.close();
    }
}
