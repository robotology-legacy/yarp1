
/* ../RawActivate.c */
int RawActivate ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../abmain.c */

/* ../base_resize.c */
int base_resize ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../dibujo.c */
void dibujo ( PtWidget_t *widget , PhTile_t *damage );

/* ../exit_viewer.c */
int exit_viewer ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../image_freeze.c */
int image_freeze ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../init.c */
int ParseParams ( int argc , char *argv []);
int init ( int argc , char *argv []);

/* ../raw_init.c */
int raw_init ( PtWidget_t *widget );

/* ../save_image.c */
int save_image ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
