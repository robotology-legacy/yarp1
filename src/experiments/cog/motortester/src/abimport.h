/* Import (extern) header for application - AppBuilder 1.14C */

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[44];

extern ApWindowLink_t file_selector_window;
extern const ApEventLink_t file_selector_window_links[];
extern ApWidget_t dsfile_selector_window[6];

#ifdef __cplusplus
extern "C" {
#endif
int init( int argc, char **argv );
int update_position_trend( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int initialize( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int quit( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis_change( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int change_gains( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int reset( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int zero_encoder( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int goto_goal( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int change_sample_rate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int enable_trigger( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int change_trend_max( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int enable_repeat( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int enable_link( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int save_filter( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int file_set_selection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int file_change_filter( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int file_accept( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int file_change_selection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int file_setup_upload( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int file_setup_download( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int change_trend_min( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int soft_shutdown( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
