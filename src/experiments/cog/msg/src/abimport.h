/* Import (extern) header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[38];


#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
int initialize( int argc, char **argv );
int quit( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int connect( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int raise_arm( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int relax_arm( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int wave_arm( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int stop_neck( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int clear_headmsg( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int arm_goto( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int start_reaching( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int stop_reaching( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_wave( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int setnoise( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int head_goto( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int start_planar( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_intermediate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_basis1( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_basis2( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int set_basis3( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int savecalibration( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int loadcalibration( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int stepforward( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggledebug( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int uncalibrate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int move_torso( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int torso_reset( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_wave3( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
