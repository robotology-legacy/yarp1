/* Import (extern) header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern const ApEventLink_t appl_links[];
extern ApEventLink_t internal_links[];

extern ApWindowLink_t base;
extern const ApEventLink_t base_links[];
extern ApWidget_t dsbase[13];

extern ApWindowLink_t player;
extern const ApEventLink_t player_links[];
extern ApWidget_t dsplayer[7];

extern ApWindowLink_t files;
extern const ApEventLink_t files_links[];
extern ApWidget_t dsfiles[8];

extern ApWindowLink_t live;
extern const ApEventLink_t live_links[];
extern ApWidget_t dslive[11];

extern ApWindowLink_t sequence;
extern const ApEventLink_t sequence_links[];
extern ApWidget_t dssequence[19];

extern ApWindowLink_t stats;
extern const ApEventLink_t stats_links[];
extern ApWidget_t dsstats[5];


#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
int initialize( int argc, char **argv );
int quit( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realize_display( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealize_display( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int display_timer_activate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int enable_recording( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_freeze_display( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int connect( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int display_sequence( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_plot1( PtWidget_t *widget, PhTile_t *damage ) ;
void draw_plot2( PtWidget_t *widget, PhTile_t *damage ) ;
int process_sequence( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int init_opticflow( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int disable_opticflow( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int enable_secret( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int disable_secret( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int browser_next_cb( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int toggle_play_from_mem( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int rewind_memory( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int back_memory( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int forward_memory( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int forward_memory_10( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int change_dir_name( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int load_data( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int save_data( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realized_dir_name( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized_dir_name( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int browser_init_cb( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int load_canonical( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int save_canonical( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int update( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int print_data( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int save( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int find_object( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
