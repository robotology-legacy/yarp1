///
/// $Id: end_pack_for_net.h,v 1.2 2003-04-10 15:01:25 gmetta Exp $
///

#include <conf/YARPConfig.h>

#ifdef __WIN__
#pragma pack(pop)
#endif

#ifdef __LINUX__
#pragma align 0
#endif

#ifdef __QNX__
#pragma  pack (pop) ;
#endif

