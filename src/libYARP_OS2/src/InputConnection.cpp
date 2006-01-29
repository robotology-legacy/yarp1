
#include <yarp/InputConnection.h>
#include <yarp/PortCommand.h>
#include <yarp/Logger.h>

using namespace yarp;

void InputConnection::run() {
  YARP_ASSERT(proto!=NULL);

  PortCommand cmd;
  
  proto->open(getName().c_str());

  bool done = false;
  while (!done) {
    BlockReader& br = proto->beginRead();
    ACE_OS::printf("InputConnection::run ready for read...\n");
    cmd.readBlock(br);
    ACE_OS::printf("InputConnection::run done with read...\n");
    if (closed) {
      done = true;
      break;
    }
    char key = cmd.getKey();
    ACE_OS::printf("Port command is [%c:%d/%s] manager? %d\n",
		   (key>=32)?key:'?', key, cmd.getText().c_str(),
		   hasManager());
    if (hasManager()) {
      PortManager& man = getManager();
      if (br.isTextMode()) {
	man.replyTo(&(proto->getOutputStream()));
      }
      switch (key) {
      case '/':
	ACE_OS::printf("asking to add output\n");
	man.addOutput(cmd.getText());
	break;
      case '!':
	man.removeOutput(cmd.getText().substring(1,-1));
	break;
      case '~':
	man.removeInput(cmd.getText().substring(1,-1));
	break;
      case '*':
	man.describe();
	break;
      case 'd':
	man.readBlock(br);
	break;
      }
      if (br.isTextMode()) {
	man.replyTo(NULL);
      }
    }
    if (key=='q') {
      done = true;
    }
    proto->endRead();
  }

  proto->close();
  delete proto;
  proto = NULL;
}

