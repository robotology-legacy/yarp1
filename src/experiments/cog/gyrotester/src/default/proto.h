
/* ../abmain.c */
int main ( int argc , char *argv []);

/* ../boresight.c */
int boresight ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../draw_cube.c */
void convert_full ( float x , float y , float z , float *xf , float *yf , float *zf );
void draw_cube ( PtWidget_t *widget , PhTile_t *damage );

/* ../initialize.c */
int initialize ( int argc , char *argv []);

/* ../timer_activate.c */
int timer_activate ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../unboresight.c */
int unboresight ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
