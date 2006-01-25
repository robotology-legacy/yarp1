
package yarp.os;

import java.io.*;
import java.net.*;
import java.util.*;

/*
  Pending issue: may need to be able to choose specific network interface
 */


class McastCarrier extends AbstractCarrier {

    /**
     * A manager class for detecting redundant connections.
     */
    static class McastManager {

	private Map tracker = new HashMap();
	private Map elected = new HashMap();
	
	public Set getPeers(String key) {
	    Set peers = (Set)tracker.get(key);
	    if (peers==null) {
		log.println(">>>>>>>>>>>> creating peer set for " + key);
		tracker.put(key,new HashSet());
		peers = (Set)tracker.get(key);
	    }
	    log.assertion(peers!=null);
	    return peers;
	}

	public void addCarrier(String key, McastCarrier carrier) {
	    log.println(">>>>>>>>>>>>>>>> add carrier for key " + key);
	    Set peers = getPeers(key);
	    log.println("pre size " + peers.size());
	    peers.add(carrier);
	    log.println("post size " + peers.size());
	    if (peers.size()==1) {
		elected.put(key,carrier);
	    }
	    log.assertion(getPeers(key).size()>0);
	}

	public void removeCarrier(String key, McastCarrier carrier) {
	    log.println(">>>>>>>>>>>>>> remove carrier for key " + key);
	    Set peers = getPeers(key);
	    peers.remove(carrier);
	    if (elected.get(key)==carrier) {
		Object next = null;
		Iterator it = peers.iterator();
		if (it.hasNext()) {
		    next = it.next();
		}
		elected.put(key,next);
	    }
	}

	public void dumpCarrier(String mcastKey) {
	    Set peers = getPeers(mcastKey);
	    log.println("************************************");
	    log.println("*** information about mcast carriers");
	    log.println("*** key is " + mcastKey + " and there are " +
			peers.size() + " senders");
	    log.println("*** hash container has " + tracker.size() +
			" elements");
	    for (Iterator it = tracker.keySet().iterator(); it.hasNext(); ) {
		String key = (String)it.next();
		log.println("   hash key " + key + " with " +
			    getPeers(key).size());
		
	    }
	    //assert(peers.size()>=1);
	    for (Iterator it = peers.iterator(); it.hasNext(); ) {
		McastCarrier c = (McastCarrier)it.next();
		log.println(c + "  " + this);
	    }
	    log.println("************************************");
	}

	public McastCarrier getElect(String key) {
	    return (McastCarrier)elected.get(key);
	}
	
    }



    private DatagramTwoWayStream way = null;

    public TwoWayStream getStreams() {
	return way;
    }

    public MulticastSocket getSocket() {
	return (MulticastSocket) way.get();
    }

    private String senderName;
    private Address mcastAddress;
    String mcastKey;
    boolean reading = false;



    static McastManager manager = new McastManager();

    public boolean isElect() {
	manager.dumpCarrier(mcastKey);
	return manager.getElect(mcastKey)==this;
    }


    public String getName() {
	return "mcast";
    }

    public int getSpecifier() {
	return 1;
	//return 98;
	//return 0x62;
    }

    public boolean checkHeader(byte[] header) {
	return getSpecifier(header)%16 == getSpecifier();
    }

    public byte[] getHeader() {
	return createStandardHeader(getSpecifier());
    }

    public boolean sendHeader(Protocol proto) throws IOException {
	super.sendHeader(proto);
	log.println("sendExtraHeader for " + getName());

	log.println("Sending mcast info");
	byte b[] = new byte[6];

	String address = mcastAddress.getName();
	//log.println("REMOTE address is " + remote);
	InetAddress inet = InetAddress.getByName(address);
	byte[] raw = inet.getAddress();
	if (raw.length!=4) {
	    log.error("Mcast address strange size");
	    System.exit(1);
	}
	for (int i=0; i<4; i++) {
	    b[i] = raw[i];
	}
	int port = mcastAddress.getPort();
	b[5] = (byte)(port%256);
	b[4] = (byte)(port/256);

	proto.write(b);
	return true;
    }

    public Address mcastQuery(String name) {
	NameClient nc = NameClient.getNameClient();
	String q = NameClient.getNamePart(name) + "-mcast";
        Address address = nc.query(q);
        //Address address = nc.probe("NAME_SERVER query " + q);
	if (address==null) {
	    return nc.register(q,"mcast");
	}
	/*
	// working on switching to a cleaner implementation
	String q = NameClient.getNamePart(name);
        String mcastIp = nc.getOne(name,"mcast.ip");
	if (mcastIp!=null) {
	}
	*/
	return address;
    }

    public void prepareSend(Protocol proto) {
	log.println("prepareSend for " + getName());
	senderName = proto.getRoute().getFromName();
	log.println("Getting mcast info");
	mcastAddress = mcastQuery(senderName);
	log.println("prepareSend: mcast address is " + mcastAddress);
    }

    public boolean expectExtraHeader(Protocol proto) throws IOException {
	log.println("expectExtraHeader for " + getName());
	log.println("Looking for mcast info");
	byte b[] = new byte[6];
	proto.read(b);
	int ip[] = new int[4];
	for (int i=0; i<4; i++) {
	    ip[i] = NetType.unsigned(b[i]);
	    log.println("  >> IP " + ip[i]);
	}
	// strange byte order
	int port = NetType.netInt(new byte[] { b[5], b[4], 0, 0 });
	log.println("  >> PORT " + port);
	
	mcastAddress = new Address("" + ip[0] + "." + 
				   ip[1] + "." +
				   ip[2] + "." +
				   ip[3],
				   port);
	return true;
    }

    public void expectReplyToHeader(Protocol proto) throws IOException {
	// in mcast case, don't expect port number
	// super.expectReplyToHeader(proto);

	//proto.become(getName(),mcastAddress);
	proto.become(this,mcastAddress);
    }

    public boolean respondToHeader(Protocol proto) throws IOException {
	//super.respondToHeader(proto);
	log.println("respondExtraToHeader for " + getName());
	log.println("TAGGING as a reading mcast");
	reading = true;
	//proto.become(getName(),mcastAddress);
	proto.become(this,mcastAddress);
	return true;
    }

    public void close() throws IOException {
	if (mcastKey!=null) {
	    manager.removeCarrier(mcastKey,this);
	    mcastKey = null;
	}
	log.println("McastCarrier.close");
	if (way!=null) {
	    log.println("McastCarrier.close socket");
	    way.close();
	    way = null;
	}
    }


    public Address makeDgram(Address address) throws IOException {
	if (way == null) {
	    MulticastSocket mcast;
	    if (address==null) {
		mcast = new MulticastSocket();
	    } else {
		mcast = new MulticastSocket(address.getPort());
	    }
	    /*
	    Address clocal = 
		new Address(NameClient.getNameClient().getHostName(),
			    mcast.getLocalPort());
	    setAddress(clocal,getRemoteAddress());
	    */
	    log.assertion(mcast!=null);
	    way = new DatagramTwoWayStream(mcast,
					   address,
					   address,
					   reading);
	}
	return getLocalAddress();
    }


    public void open(Address address, ShiftStream previous) throws IOException {
	address = new Address(address.getName(),
			      address.getPort(),
			      "mcast");
	log.println("********* open for mcast address " + address);
	Address clocal = previous.getLocalAddress();
	Address tcpRemote = previous.getRemoteAddress();

	previous.close();

	Address cremote = address;
	InetAddress group = InetAddress.getByName(address.getName());
	InetAddress tcp = InetAddress.getByName(tcpRemote.getName());

	clocal = makeDgram(cremote);

	//if (mcast==null) {
	//  mcast = new MulticastSocket(address.getPort()); 
	//}

	log.debug("tcp address is " + tcpRemote);
	log.debug("mcast address is " + cremote);

	if (!reading) {
	    NetworkInterface anticipate = null;
	    try {
		anticipate = NetworkInterface.getByInetAddress(tcp);

		log.debug("anticipate nic is " + anticipate.getDisplayName());
		if (anticipate.getDisplayName().equals("lo")) {
		    // loopback may not support multicast, so skip setting interface
		} else {
		    // setting the interface seems to break things right now.
		    getSocket().setNetworkInterface(anticipate);
		}

	    } catch (SocketException e) {
		log.println("safe to ignore: " + e);
	    }
	}

	log.debug("joining group " + group);
	getSocket().joinGroup(group);
	NetworkInterface nic = null;
	String nicName = "none";
	try {
	    nic = getSocket().getNetworkInterface();
	    nicName = nic.getDisplayName();
	} catch (SocketException e) {
	    log.println("safe to ignore: " + e);
	}
	log.println("Nic name is " + nicName);

	mcastKey = mcastAddress.getName() + " on NIC " + nicName;

	if (!reading) {
	    manager.addCarrier(mcastKey,this);
	}

	/*
	setAddress(clocal,cremote);
	is = new DatagramInputStream(mcast,512);
	os = new BufferedOutputStream(new DatagramOutputStream(mcast,
							       cremote,
							       512,
							       reading),
				      512);
	*/
    }


    public boolean isActive() throws IOException {
	// should check if output is necessary
	return isElect();
    }

    public Carrier create() {
	log.println("*** McastCarrier::create()");
	return new McastCarrier();
    }

    public void start(Address address) throws IOException {
	log.println("Starting mcast, I hope you know what you're doing");
	makeDgram(address);
	InetAddress group = InetAddress.getByName(address.getName());
	log.debug("joining group " + group);
	getSocket().joinGroup(group);
    }
}

