#ifndef wide_nameloc_INC
#define wide_nameloc_INC

#include "YARPNetworkTypes.h"

enum NameServiceRequest
{
  NAME_NO_ACTION,
  NAME_REGISTER,
  NAME_LOOKUP,
};

#include "begin_pack_for_net.h"
struct NameServiceHeader
{
  NetInt32 port;
  NetInt32 key_length;
  NetInt32 machine_length;
  NetInt32 spare1;
  NetInt32 spare2;
  char request_type;
} PACKED_FOR_NET;
#include "end_pack_for_net.h"



#endif
