/* Import (extern) header for application - AppBuilder 2.01  */

#include "abdefine.h"

extern ApWindowLink_t base;
extern ApWidget_t AbWidgets[ 22 ];


#ifdef __cplusplus
extern "C" {
#endif
int init( int argc, char **argv );
int slider_bright_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int slider_hue_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int slider_contrast_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int slider_satu_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int slider_satv_mode( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int base_open( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_lnotch_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_ldec_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_crush_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_peak_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_cagc_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_ckill_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_range_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_ysleep_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_csleep_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_gamma_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int tb_dithframe_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
