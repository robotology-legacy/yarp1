
/* ../abmain.c */
int main ( int argc , char *argv []);

/* ../axis_change.c */
int axis_change ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../calibrate.c */
double VelocityMove ( int axis , double vel , double acc , double errlimit );
int CalibrateAxis ( int axis );
int calibrate ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../change_gains.c */
void draw_gains ( void );
int change_gains ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../change_sample_rate.c */
int change_sample_rate ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../change_trend_max.c */
int change_trend_max ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../change_trend_min.c */
int change_trend_min ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../enable_link.c */
void set_link ( int update_button_state );
void remove_link ( int update_button_state );
int enable_link ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../enable_repeat.c */
void set_repeat ( int update_button_state );
void remove_repeat ( int update_button_state );
int enable_repeat ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../enable_trigger.c */
void set_trigger ( int update_button_state );
void remove_trigger ( int update_button_state );
int enable_trigger ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../file_accept.c */
int upload ( char *fname );
int download ( char *fname );
int file_accept ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../file_change_filter.c */
int file_change_filter ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../file_change_selection.c */
int file_change_selection ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../file_set_selection.c */
int file_set_selection ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../file_setup_download.c */
int file_setup_download ( PtWidget_t *link_instance , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../file_setup_upload.c */
int file_setup_upload ( PtWidget_t *link_instance , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../goto_goal.c */
int goto_goal ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../init.c */
void ParseCommandLine ( int argc , char *argv []);
int init ( int argc , char *argv []);

/* ../initialize.c */
void disable_widget ( PtWidget_t *widget_name );
void enable_widget ( PtWidget_t *widget_name );
int read_integer_from_widget ( PtWidget_t *widget );
double read_double_from_widget ( PtWidget_t *widget );
char *read_text_from_widget ( PtWidget_t *widget );
int read_selected_item_from_widget ( PtWidget_t *widget );
int initialize ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../quit.c */
int quit ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../reset.c */
int reset ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../save_filter.c */
void print_pid_filter ( short *pid );
int save_filter ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../soft_shutdown.c */
int soft_shutdown ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../update_position_trend.c */
int update_position_trend ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );

/* ../zero_encoder.c */
int zero_encoder ( PtWidget_t *widget , ApInfo_t *apinfo , PtCallbackInfo_t *cbinfo );
