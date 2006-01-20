
package yarp.os;

class Route {
    private String fromKey = null;
    private String toKey = null;
    private String carrier = null;

    public Route() {
    }
    
    public Route(String fromKey, 
		 String toKey,
		 String carrier) {
	this.fromKey = fromKey;
	this.toKey = toKey;
	this.carrier = carrier;
    }

    public String getFromName() {
	return fromKey;
    }

    public String getToName() {
    	return toKey;
    }

    public String getCarrierName() {
	return carrier;
    }

    public Route addFromName(String from) {
	return new Route(from,getToName(),getCarrierName());
    }

    public Route addToName(String to) {
	return new Route(getFromName(),to,getCarrierName());
    }

    public Route addCarrierName(String carrier) {
	return new Route(getFromName(),getToName(),carrier);
    }

    public String toString() {
	return getFromName() + "->" + getCarrierName() + "->" +
	    getToName();
    }
}

