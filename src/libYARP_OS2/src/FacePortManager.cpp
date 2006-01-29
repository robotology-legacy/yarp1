
#include <yarp/FacePortManager.h>
#include <yarp/InputProtocol.h>
#include <yarp/Logger.h>

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

