/* Import (extern) header for application - AppBuilder 2.01  */

#include "abdefine.h"

extern ApWindowLink_t base;
extern ApWidget_t AbWidgets[ 9 ];

extern ApMenuLink_t filemenu;
extern ApMenuLink_t imagemenu;

#ifdef __cplusplus
extern "C" {
#endif
int init( int argc, char **argv );
int RawActivate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void dibujo( PtWidget_t *widget, PhTile_t *damage ) ;
int raw_init( PtWidget_t *widget ) ;
int exit_viewer( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int image_freeze( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int save_image( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int base_resize( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
