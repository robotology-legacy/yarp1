/* Import (extern) header for application - AppBuilder 1.14C */

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[13];

#ifdef __cplusplus
extern "C" {
#endif
int initialize( int argc, char **argv );
int timer_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_cube( PtWidget_t *widget, PhTile_t *damage ) ;
int boresight( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unboresight( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
