
#include <yarp/FacePortManager.h>
#include <yarp/InputProtocol.h>
#include <yarp/Logger.h>
#include <yarp/BufferedBlockWriter.h>

// temporary
#include <yarp/PortCommand.h>
#include <yarp/Bottle.h>
#include <yarp/NameClient.h>

using namespace yarp;

void FacePortManager::run() {
  YARP_ASSERT(face!=NULL);
  while (true) {
    InputProtocol *ip = face->read();
    addInput(ip);
  }
}

void FacePortManager::readBlock(BlockReader& reader) {
  ACE_OS::printf("readBlock!\n");
  if (this->reader!=NULL) {
  ACE_OS::printf("passing up the read chain\n");
    this->reader->readBlock(reader);
  }
}


void FacePortManager::describe() {
  ACE_OS::printf("FacePortManager::describe\n");
  if (hasOutput()) {
    OutputStream& os = getOutputStream();
    BufferedBlockWriter bw(true);
    bw.appendLine(String("This is ") + name + " at " + address.toString());
    bw.appendLine("There are no outgoing connections");
    int ict = inputs.size();
    if (ict<1) {
      bw.appendLine("There are no incoming connections");
    } else {
      for (int i=0; i<ict; i++) {
	InputEntry *entry = inputs[i];
	YARP_ASSERT(entry!=NULL);
	
	Route route = entry->getConnection().getRoute();
	String msg = "There is an input connection from " + 
	  route.getFromName() +
	  " to " + route.getToName() + " using " + 
	  route.getCarrierName();
	bw.appendLine(msg);
      }
    }
    bw.write(os);
  }
}


void FacePortManager::removeInput(const String& src) {
  BufferedBlockWriter bw(true);
  bool success = false;
  for (int i=0; i<inputs.size(); i++) {
    InputEntry *entry = inputs[i];
    YARP_ASSERT(entry!=NULL);
    Route route = entry->getConnection().getRoute();
    if (route.getFromName() == src) {
      entry->getConnection().askForClose();
      bw.appendLine(String("Removing connection from ") + route.getFromName() +
		    " to " + route.getToName());
      success = true;
      break;
    }
  }
  if (!success) {
    bw.appendLine(String("Could not find an input connection from ") + src +
		  " to " + getName());
  }
  if (hasOutput()) {
    bw.write(getOutputStream());
  }
}


void FacePortManager::removeOutput(const String& src) {
  BufferedBlockWriter bw(true);
  bw.appendLine(String("Should do something about removing output ") + src);
  if (hasOutput()) {
    bw.write(getOutputStream());
  }
}

void FacePortManager::addOutput(const String& src) {
  BufferedBlockWriter bw(true);
  bw.appendLine(String("Should do something about adding output ") + src);

  Address address = NameClient::getNameClient().queryName(src);
  ACE_OS::printf("Should connect to address %s\n", address.toString().c_str());
  OutputProtocol *out = Carriers::connect(address);
  if (out!=NULL) {
    BufferedBlockWriter buf(true);
    PortCommand pc('d',"");
    pc.writeBlock(buf);
    Bottle bot;
    bot.fromString("0 \"hello\"");
    bot.writeBlock(buf);
    Route route("/here","/there","text");
    out->open(route);
    if (out->isActive()) {
      out->write(buf);
    }
  }
  delete out;

  if (hasOutput()) {
    bw.write(getOutputStream());
  }
}



void FacePortManager::InputEntry::run() {
  try {
    connection.run();
  } catch (IOException e) {
    ACE_OS::printf("an input connection complained: %s\n", 
		   e.toString().c_str());
  }
  ACE_OS::printf("an input connection stopped\n");

  // need to make this entry vanish
  owner->removeInput(this);
}


void FacePortManager::InputEntry::close() {
  connection.close();
}

