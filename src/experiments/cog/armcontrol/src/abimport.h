/* Import (extern) header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[37];


#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
int initialize( int argc, char **argv );
int quit_application( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realized( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate_left( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int raise_gain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int zero_gain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_time( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_timer( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realize_trend( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_trend( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int print_position( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_alpha( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int print_axis( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int do_test_motion( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis_changed( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_tgain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_pgain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int gravitysave( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int gravitylearn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int gravityactivate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int torsoraisegain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int torsozerogain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int settorsogain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int connectbody( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_dtorque( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
