/* Import (extern) header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[107];


#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
int Initialize( int argc, char **argv );
int ResetPIC( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MEIDreset( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis0slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis1slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis2slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis3slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis4slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis5slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis6slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis7slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axischange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int quit( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int zero_encoder( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis8slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis9slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis10slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis11slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis12slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis13slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis14slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis29slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis28slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis27slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis26slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis25slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis24slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis23slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis22slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis21slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis20slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis19slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis18slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis17slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis16slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int axis15slider_move( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveToPos( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis1Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis2Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis3Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis4Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis5Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis6Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis7Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis8Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis9Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis10Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis11Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis12Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis13Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis14Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis15Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis16Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis17Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis18Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis19Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis20Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis21Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis22Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis23Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis24Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis25Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis26Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis27Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis28Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis29Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Axis0Toggle( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend1AxisChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend1MaxChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend1MinChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend2AxisChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend3AxisChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend4AxisChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int TimerActivate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Realized( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int TurnOnAll( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int TurnOffAll( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend3MaxChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend3MinChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend4MaxChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend4MinChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend5MaxChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend5MinChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend2MaxChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Trend2MinChange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetVelocity( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int CalibrateAll( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
