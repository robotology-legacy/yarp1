
package yarp.os;

import java.io.*;

/**
 * Multi-cast server, for last resort information sharing about
 * name information -- when config files are missing or wrong
 */
class FallbackServer extends Thread {
    static private Logger log = new Logger("FallbackServer: ",Logger.get());

    private YarpServer server = null;

    public FallbackServer(YarpServer server) {
	this.server = server;
    }

    private static final Address mcastLastResort =
	new Address("224.2.1.1",10001,"mcast","fallback");

    public static Address getAddress() {
	return mcastLastResort;
    }

    public void run() {
	try {
	    Carrier car = new McastCarrier();
	    car.start(mcastLastResort);
	    InputStream in = car.getStreams().getInputStream();
	    OutputStream out = car.getStreams().getOutputStream();
	    while (true) {
		String txt = NetType.readLine(in);
		log.info("got " + txt);
		if (txt.length()>=1) {
		    if (txt.charAt(0)=='N') {
			/*
			Address addr =
			    NameClient.getNameClient().getAddress();
			out.write(NetType.netString(YarpServer.textify(addr)));
			*/
			Address remote = car.getStreams().getRemoteAddress();
			String result = server.apply(txt,remote);
			out.write(NetType.netString(result));
			out.flush();
		    }
		}
	    }
	} catch (IOException e) {
	    log.info("mcast turner " + e);
	}
    }
}

