
#include <yarp/NameServer.h>
#include <yarp/Logger.h>

using namespace yarp;


Address NameServer::registerName(const String& name, 
				 const Address& address) {
  NameRecord& nameRecord = getNameRecord(name);
  nameRecord.setAddress(address.addRegName(name));
  return nameRecord.getAddress();
}


Address NameServer::queryName(const String& name) {
  NameRecord *rec = getNameRecord(name,false);
  if (rec!=NULL) {
    return rec->getAddress();
  }
  return Address();
}


NameServer::NameRecord *NameServer::getNameRecord(const String& name, 
						  bool create) {
  ACE_Hash_Map_Entry<String,NameRecord> *entry;
  int result = nameMap.find(name,entry);
  if (result==-1) {
    if (!create) {
      return NULL;
    }
    nameMap.bind(name,NameRecord());
    result = nameMap.find(name,entry);
  }
  YARP_ASSERT(result!=-1);
  return &(entry->int_id_);
}

