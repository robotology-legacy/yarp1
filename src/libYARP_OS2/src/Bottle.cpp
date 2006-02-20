#include <yarp/BottleImpl.h>
#include <yarp/Logger.h>
#include <yarp/IOException.h>

#include <yarp/Bottle.h>

#include <yarp/Adapters.h>

using namespace yarp;
using namespace yarp::os;

// implementation is a BottleImpl
#define HELPER(x) (*((BottleImpl*)(x)))

Bottle::Bottle() {
  implementation = new BottleImpl;
  YARP_ASSERT(implementation!=NULL);
}

Bottle::Bottle(const char *text) {
  implementation = new BottleImpl;
  YARP_ASSERT(implementation!=NULL);
  fromString(text);
}

Bottle::~Bottle() {
  if (implementation!=NULL) {
    delete &HELPER(implementation);
    implementation = NULL;
  }
}

void Bottle::clear() {
  HELPER(implementation).clear();
}

void Bottle::addInt(int x) {
  HELPER(implementation).addInt(x);
}

void Bottle::addDouble(double x) {
  HELPER(implementation).addDouble(x);
}

void Bottle::addString(const char *str) {
  HELPER(implementation).addString(str);
}

int Bottle::getInt(int index) {
  return HELPER(implementation).getInt(index);
}

ConstString Bottle::getString(int index) {
  return ConstString(HELPER(implementation).getString(index).c_str());
}

double Bottle::getDouble(int index) {
  return HELPER(implementation).getDouble(index);
}

bool Bottle::isInt(int index) {
  return HELPER(implementation).isInt(index);
}

bool Bottle::isDouble(int index) {
  return HELPER(implementation).isDouble(index);
}

bool Bottle::isString(int index) {
  return HELPER(implementation).isString(index);
}

void Bottle::fromString(const char *text) {
  HELPER(implementation).fromString(text);
}

ConstString Bottle::toString() {
  return ConstString(HELPER(implementation).toString().c_str());;
}

bool Bottle::write(ConnectionWriter& writer) {
  bool result = false;
  try {
    BlockWriterAdapter adapter(writer);
    HELPER(implementation).writeBlock(adapter);
    result = true;
  } catch (IOException e) {
    YARP_DEBUG(Logger::get(), e.toString() + " <<< Bottle::write saw this");
    // leave result false
  }
  return result;
}


bool Bottle::read(ConnectionReader& reader) {
  bool result = false;
  try {
    BlockReaderAdapter adapter(reader);
    HELPER(implementation).readBlock(adapter);
    result = true;
  } catch (IOException e) {
    YARP_DEBUG(Logger::get(), e.toString() + " <<< Bottle::read saw this");
    // leave result false
  }
  return result;
}

