
/* ../abmain.cc */
int main ( int argc , char *argv []);

/* ../exit_callback.cc */
int exit_callback ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../file.cc */
void *ImageFile_alloc ( long nbytes , int type );
void *ImageFile_free ( void *mem , int type );
void *ImageFile_error ( char *msg );
void *ImageFile_warning ( char *msg );
void *ImageFile_progress ( int percent );
int save ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
int load ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../flurry_init.cc */
void ParseCommandLine ( int argc , char *argv []);
int ExitSignalHandler ( int signum , void *data );
int flurry_init ( int argc , char *argv []);

/* ../freeze.cc */
void set_freeze ( void );
int freeze ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../resize.cc */
void set_size ( void );
int resize_bigger ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
int resize_smaller ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
int initial_size ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../set_file.cc */
char *read_text_from_widget ( PtWidget_t *widget );
int set_selection ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
int change_selection ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
int change_filter ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
int realized ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
int unrealized ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../trajectory_color.cc */
int trajectory_color ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../viewer_draw.cc */
void SetupImage ( PhImage_t *im , int w , int h , int bytes_per_pixel );
void setup_DB ( int w , int h );
void DrawTrajectories ( int isClear );
int viewer_draw ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
