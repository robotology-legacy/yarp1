
#include <yarp/BottleImpl.h>
#include <yarp/BufferedBlockWriter.h>
#include <yarp/StreamBlockReader.h>
#include <yarp/StringOutputStream.h>
#include <yarp/StringInputStream.h>

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

using namespace yarp;

const int BottleImpl::StoreInt::code = 1;
const int BottleImpl::StoreString::code = 5;
const int BottleImpl::StoreDouble::code = 2;


BottleImpl::BottleImpl() {
  dirty = true;
}


BottleImpl::~BottleImpl() {
  clear();
}


void BottleImpl::add(Storable *s) {
  content.push_back(s);
  dirty = true;
}


void BottleImpl::clear() {
  for (unsigned int i=0; i<content.size(); i++) {
    delete content[i];
  }
  content.clear();
  dirty = true;
}

void BottleImpl::smartAdd(const String& str) {
  if (str.length()>0) {
    char ch = str[0];
    Storable *s = NULL;
    if (ch>='0'&&ch<='9'||ch=='+'||ch=='-'||ch=='.') {
      if (str.strstr(".")<0) {
	s = new StoreInt(0);
      } else {
	s = new StoreDouble(0);
      }
    } else {
      s = new StoreString("");
    }
    if (s!=NULL) {
      s->fromString(str);
      //ACE_OS::printf("*** smartAdd [%s] [%s]\n", str.c_str(), s->toString().c_str());
      add(s);
    }
  }
}

void BottleImpl::fromString(const String& line) {
  clear();
  dirty = true;
  String arg = "";
  bool quoted = false;
  bool back = false;
  bool begun = false;
  String nline = line + " ";
  for (unsigned int i=0; i<nline.length(); i++) {
    char ch = nline[i];
    if (back) {
      arg += ch;
      back = false;
    } else {
      if (!begun) {
	if (ch!=' '&&ch!='\t') {
	  begun = true;
	}
      }
      if (begun) {
	if (ch=='\"') {
	  if (!quoted) {
	    quoted = true;
	  } else {
	    quoted = false;
	  }
	}
	if (ch=='\\') {
	  back = true;
	  arg += ch;
	} else {
	  if ((!quoted)&&(ch==' '||ch=='\t')) {
	    smartAdd(arg);
	    arg = "";
	    begun = false;
	  } else {
	    arg += ch;
	  }
	}
      }
    }
  }
}

String BottleImpl::toString() {
  String result = "";
  for (unsigned int i=0; i<content.size(); i++) {
    if (i>0) { result += " "; }
    Storable& s = *content[i];
    result += s.toString();
  }
  return result;
}

int BottleImpl::size() {
  return content.size();
}

void BottleImpl::fromBytes(const Bytes& data) {
  String wrapper;
  wrapper.set(data.get(),data.length(),0);
  StringInputStream sis;
  sis.add(wrapper);
  StreamBlockReader reader;
  reader.reset(sis,NULL,data.length(),false);

  clear();
  dirty = true; // for clarity

  try {
    while (reader.getSize()>0) {
      int id = reader.expectInt();
      Storable *storable = NULL;
      switch (id) {
      case StoreInt::code:
	storable = new StoreInt();
	break;
      case StoreDouble::code:
	storable = new StoreDouble();
	break;
      case StoreString::code:
	storable = new StoreString();
	break;
      }
      if (storable==NULL) {
	YARP_ERROR(Logger::get(), "BottleImpl reader failed");
	throw IOException("BottleImpl reader failed - unrecognized format?");
      }
      storable->readBlock(reader);
      add(storable);
    }
  } catch (IOException e) {
    YARP_DEBUG(Logger::get(), e.toString() + " bottle reader stopped");
  }
}

void BottleImpl::toBytes(const Bytes& data) {
  synch();
  YARP_ASSERT(data.length()==byteCount());
  ACE_OS::memcpy(data.get(),getBytes(),byteCount());
}


const char *BottleImpl::getBytes() {
  synch();
  return &data[0];
}

int BottleImpl::byteCount() {
  synch();
  return data.size();
}

void BottleImpl::writeBlock(BlockWriter& writer) {
  // could simplify this if knew lengths of blocks up front
  if (writer.isTextMode()) {
    writer.appendLine(toString());
  } else {
    String name = "YARP2";
    writer.appendInt(name.length()+1);
    writer.appendString(name);
    synch();
    writer.appendInt(byteCount());
    writer.appendBlockCopy(Bytes((char*)getBytes(),byteCount()));
  }
}


void BottleImpl::readBlock(BlockReader& reader) {
  if (reader.isTextMode()) {
    String str = reader.expectLine();
    fromString(str);
  } else {
    int len = reader.expectInt();
    String name = reader.expectString(len);
    int bct = reader.expectInt();
    ManagedBytes b(bct);
    reader.expectBlock(b.bytes());
    fromBytes(b.bytes());
  }
}


void BottleImpl::synch() {
  if (dirty) {
    data.clear();
    //StringOutputStream sos;
    BufferedBlockWriter writer;
    for (unsigned int i=0; i<content.size(); i++) {
      Storable *s = content[i];
      writer.appendInt(s->getCode());
      s->writeBlock(writer);
    }
    //buf.write(sos);
    String str = writer.toString();
    data.resize(str.length(),' ');
    memcpy(&data[0],str.c_str(),str.length());
    dirty = false;
  }
}



////////////////////////////////////////////////////////////////////////////
// StoreInt

String BottleImpl::StoreInt::toString() {
  char buf[256];
  ACE_OS::sprintf(buf,"%d",x);
  return String(buf);
}

void BottleImpl::StoreInt::fromString(const String& src) {
  x = ACE_OS::atoi(src.c_str());
}

void BottleImpl::StoreInt::readBlock(BlockReader& reader) {
  x = reader.expectInt();
}

void BottleImpl::StoreInt::writeBlock(BlockWriter& writer) {
  writer.appendInt(x);
}


////////////////////////////////////////////////////////////////////////////
// StoreDouble

String BottleImpl::StoreDouble::toString() {
  char buf[256];
  ACE_OS::sprintf(buf,"%g",x);
  String str(buf);
  if (str.strstr(".")<0) {
    str += ".0";
  }
  return str;
}

void BottleImpl::StoreDouble::fromString(const String& src) {
  x = ACE_OS::strtod(src.c_str(),NULL);
}

void BottleImpl::StoreDouble::readBlock(BlockReader& reader) {
  reader.expectBlock(Bytes((char*)&x,sizeof(x)));
}

void BottleImpl::StoreDouble::writeBlock(BlockWriter& writer) {
  writer.appendBlockCopy(Bytes((char*)&x,sizeof(x)));
}


////////////////////////////////////////////////////////////////////////////
// StoreString


String BottleImpl::StoreString::toString() {
  // quoting code: very inefficient, but portable
  String result;
  result += "\"";
  for (unsigned int i=0; i<x.length(); i++) {
    char ch = x[i];
    if (ch=='\\'||ch=='\"') {
      result += '\\';
    }
    result += ch;
  }
  result += "\"";
  return result;
}

void BottleImpl::StoreString::fromString(const String& src) {
  // unquoting code: very inefficient, but portable
  String result = "";
  x = "";
  int len = src.length();
  if (len>0) {
    bool skip = false;
    bool back = false;
    if (src[0]=='\"') {
      skip = true;
    }
    for (int i=0; i<len; i++) {
      if (skip&&(i==0||i==len-1)) {
	// omit
      } else {
	char ch = src[i];
	if (ch=='\\') {
	  if (!back) {
	    back = true;
	  } else {
	    x += '\\';
	    back = false;
	  }
	} else {
	  back = false;
	  x += ch;
	}
      }
    }
  }
}


void BottleImpl::StoreString::readBlock(BlockReader& reader) {
  int len = reader.expectInt();
  x = reader.expectString(len);
}

void BottleImpl::StoreString::writeBlock(BlockWriter& writer) {
  writer.appendInt(x.length()+1);
  writer.appendString(x);
}




bool BottleImpl::isInt(int index) {
  if (index>=0 && index<size()) {
    return content[index]->getCode() == StoreInt::code;
  }
  return false;
}


bool BottleImpl::isString(int index) {
  if (index>=0 && index<size()) {
    return content[index]->getCode() == StoreString::code;
  }
  return false;
}

bool BottleImpl::isDouble(int index) {
  if (index>=0 && index<size()) {
    return content[index]->getCode() == StoreDouble::code;
  }
  return false;
}


int BottleImpl::getInt(int index) {
  if (!isInt(index)) { return 0; }
  return content[index]->asInt();
}

String BottleImpl::getString(int index) {
  if (!isString(index)) { return ""; }
  return content[index]->asString();
}

double BottleImpl::getDouble(int index) {
  if (!isDouble(index)) { return 0; }
  return content[index]->asDouble();
}



