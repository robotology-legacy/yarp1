/* Import (extern) header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[36];


#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
int initialize( int argc, char **argv );
int realize( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int quit( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate7( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate6( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate_diff( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int spawn_gyro( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate1( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate2( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int calibrate3( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int print_encoder( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int attentionconnectall( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int attentionconnectcontrol( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int print_limits( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_control( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int show_time( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int gotopos( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realized( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int timer_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_trend( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_fb_gain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int eyes_tilt_gain_realized( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_sac_gain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_disp_gain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int parkhead( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unparkhead( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int raisegain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int lowergain( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int connect_body( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
