/* Import (extern) header for application - AppBuilder 1.13X */

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[17];

#ifdef __cplusplus
extern "C" {
#endif
int TestButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ExitButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int StartButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int StopButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int KillButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MakeButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int Ticker_Tick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int HardTicker_Tick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int HardButton_Click( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MaintainFlag_Click( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int PowerButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int KillAllButton_OnClick( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
