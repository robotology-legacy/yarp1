#ifndef BLOCKPREFIX_INC
#define BLOCKPREFIX_INC

#include "YARPNetworkTypes.h"


#include "begin_pack_for_net.h"
class BlockPrefix
{
public:
  unsigned char total_blocks, reply_blocks;
  NetInt32 size, reply_size;
} PACKED_FOR_NET;
#include "end_pack_for_net.h"

#endif

