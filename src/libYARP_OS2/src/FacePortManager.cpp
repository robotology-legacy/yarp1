
#include <yarp/FacePortManager.h>
#include <yarp/InputProtocol.h>
#include <yarp/Logger.h>
#include <yarp/BufferedBlockWriter.h>
#include <yarp/Time.h>

// temporary
#include <yarp/PortCommand.h>
#include <yarp/Bottle.h>
#include <yarp/NameClient.h>

using namespace yarp;

void FacePortManager::run() {
  YARP_ASSERT(face!=NULL);
  //ACE_OS::printf("FacePortManager::running at %s\n",
  //	 address.toString().c_str());
  while (!closed) {
    try {
      InputProtocol *ip = face->read();
      if (ip==NULL) break;
      if (closed) {
	ip->close();
	delete ip;
	break;
      }
      addInput(ip);
    } catch (IOException e) {
      ACE_DEBUG((LM_DEBUG,"FacePortManager shutting down: %s\n",
		 e.toString().c_str()));
      break;
    }
  }
  //ACE_OS::printf("FacePortManager::run shutting down\n");
  closeResources();
}

void FacePortManager::readBlock(BlockReader& reader) {
  mutex.wait();

  //ACE_OS::printf("readBlock!\n");
  if (this->reader!=NULL) {
    //ACE_OS::printf("passing up the read chain\n");
    this->reader->readBlock(reader);
  }

  mutex.post();
}


void FacePortManager::describe() {
  mutex.wait();

  ACE_OS::printf("FacePortManager::describe\n");
  if (hasOutput()) {
    OutputStream& os = getOutputStream();
    BufferedBlockWriter bw(true);
    bw.appendLine(String("This is ") + name + " at " + address.toString());

    int oct = outputs.size();
    if (oct<1) {
      bw.appendLine("There are no outgoing connections");
    } else {
      for (int i=0; i<oct; i++) {
	OutputEntry *entry = outputs[i];
	YARP_ASSERT(entry!=NULL);
	
	Route route = entry->getRoute();
	String msg = "There is an output connection from " + 
	  route.getFromName() +
	  " to " + route.getToName() + " using " + 
	  route.getCarrierName();
	bw.appendLine(msg);
      }
    }


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

  mutex.post();
}

void FacePortManager::send(Writable& writer) {
  // issue: may need text + binary form
  BufferedBlockWriter bw(true);
  PortCommand pc('d',"");
  pc.writeBlock(bw);
  writer.writeBlock(bw);
  
  for (unsigned int i=0; i<outputs.size(); i++) {
    OutputEntry *entry = outputs[i];
    YARP_ASSERT(entry!=NULL);
    entry->write(bw);
  }
}


void FacePortManager::removeInput(const String& src) {
  mutex.wait();

  BufferedBlockWriter bw(true);
  bool success = false;
  for (unsigned int i=0; i<inputs.size(); i++) {
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
  
  mutex.post();
}


void FacePortManager::removeOutput(const String& src) {
  mutex.wait();

  BufferedBlockWriter bw(true);
  bw.appendLine(String("Should do something about removing output ") + src);
  bool success = false;
  for (unsigned int i=0; i<outputs.size(); i++) {
    OutputEntry *entry = outputs[i];
    YARP_ASSERT(entry!=NULL);
    Route route = entry->getRoute();
    if (route.getToName() == src) {
      entry->askForClose();
      bw.appendLine(String("Removing connection from ") + route.getFromName() +
		    " to " + route.getToName());
      success = true;
      break;
    }
  }
  if (!success) {
    bw.appendLine(String("Could not find an output connection from ") + 
		  getName() +
		  " to " + src);
  }
  if (hasOutput()) {
    bw.write(getOutputStream());
  }
  
  mutex.post();
}

void FacePortManager::addOutput(const String& src) {

  mutex.wait();

  BufferedBlockWriter bw(true);
  bw.appendLine(String("Adding output to ") + src);

  Address address = NameClient::getNameClient().queryName(src);
  //ACE_OS::printf("Should connect to address %s\n", address.toString().c_str());
  OutputProtocol *out = Carriers::connect(address);

  if (out!=NULL) {
    addOutput(src,out);
  }

  if (hasOutput()) {
    bw.write(getOutputStream());
  }

  mutex.post();
}




void FacePortManager::InputEntry::run() {
  try {
    connection.run();
  } catch (IOException e) {
    ACE_OS::printf("an input connection complained: %s\n", 
		   e.toString().c_str());
  }
  ACE_OS::printf("an input connection stopped\n");
  connection.close();

  // need to make this entry vanish
  owner->removeInput(this);
}


void FacePortManager::InputEntry::close() {
  connection.close();
}


void FacePortManager::OutputEntry::write(SizedWriter& writer) {
  if (op!=NULL) {
    //PortCommand pc('d',"");
    //writable.writeBlock(pc);
    //pc.writeBlock(writer);
    if (op->isActive()) {
      ACE_OS::printf("about to write...\n");
      op->write(writer);
      ACE_OS::printf("...wrote\n");
      Time::delay(0.5);
      ACE_OS::printf("...wrote 2\n");
    }
  }
}


