/* Import (extern) header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[18];


#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
int initialize( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
extern "C" {
#endif
int do_target1( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_target2( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_target3( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_target5( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_target4( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_revert1( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_revert2( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_revert3( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_revert4( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_revert5( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int go_point( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_reset( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_appear1( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_appear2( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_appear3( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_appear4( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_appear5( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
