/* Import (extern) header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[13];

extern ApWindowLink_t file_selection_window;
extern const ApEventLink_t file_selection_window_links[];
extern ApWidget_t dsfile_selection_window[5];

extern ApWindowLink_t progress_bar;
extern const ApEventLink_t progress_bar_links[];
extern ApWidget_t dsprogress_bar[3];


#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
int flurry_init( int argc, char **argv );
int exit_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int resize_bigger( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int resize_smaller( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int viewer_draw( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int initial_size( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int freeze( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_selection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int change_selection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int change_filter( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int save( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int load( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int trajectory_color( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realized( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
