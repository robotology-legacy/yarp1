


#ifndef _UEID_H_
#define _UEID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
//#include "win2qnx.h"
//#include "powerdaq.h"
//#include "pd-int.h"
//#include "pdfw_def.h"
//#include "pdfw_if.h"

/* initialization */
int ueid_initialize(char *ueid_name, int *ueidx);

/* message-passing wrappers */
int ueid_init(int ueidx);
int ueid_get_samples(int ueidx, int board, int max_samples, uint16_t *buffer);
int ueid_sw_cl_start(int ueidx, int board);

#ifdef __cplusplus
}
#endif

#endif /* _UEID_H_ */

