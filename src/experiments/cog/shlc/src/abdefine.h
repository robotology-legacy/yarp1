/* Define header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

/* Internal Module Links */

/* 'base' Window link */
extern const int ABN_base;
#define ABW_base                             dsbase[ABN_base].wgt
extern const int ABN_base_quit;
#define ABW_base_quit                        dsbase[ABN_base_quit].wgt
extern const int ABN_base_connect;
#define ABW_base_connect                     dsbase[ABN_base_connect].wgt
extern const int ABN_base_enable_recording;
#define ABW_base_enable_recording            dsbase[ABN_base_enable_recording].wgt
extern const int ABN_base_process;
#define ABW_base_process                     dsbase[ABN_base_process].wgt
extern const int ABN_base_init_of;
#define ABW_base_init_of                     dsbase[ABN_base_init_of].wgt
extern const int ABN_base_clear_of;
#define ABW_base_clear_of                    dsbase[ABN_base_clear_of].wgt
extern const int ABN_base_opticflow_on;
#define ABW_base_opticflow_on                dsbase[ABN_base_opticflow_on].wgt
extern const int ABN_base_secret;
#define ABW_base_secret                      dsbase[ABN_base_secret].wgt
extern const int ABN_base_disablesecret;
#define ABW_base_disablesecret               dsbase[ABN_base_disablesecret].wgt
extern const int ABN_base_open_files_w;
#define ABW_base_open_files_w                dsbase[ABN_base_open_files_w].wgt
extern const int ABN_base_canonical_stat;
#define ABW_base_canonical_stat              dsbase[ABN_base_canonical_stat].wgt
extern const int ABN_base_find;
#define ABW_base_find                        dsbase[ABN_base_find].wgt

/* 'player' Window link */
extern const int ABN_player;
#define ABW_player                           dsplayer[ABN_player].wgt
extern const int ABN_player_rewind_memory;
#define ABW_player_rewind_memory             dsplayer[ABN_player_rewind_memory].wgt
extern const int ABN_player_back_memory;
#define ABW_player_back_memory               dsplayer[ABN_player_back_memory].wgt
extern const int ABN_player_frorward_memory;
#define ABW_player_frorward_memory           dsplayer[ABN_player_frorward_memory].wgt
extern const int ABN_player_forward_10;
#define ABW_player_forward_10                dsplayer[ABN_player_forward_10].wgt
extern const int ABN_player_frame_no;
#define ABW_player_frame_no                  dsplayer[ABN_player_frame_no].wgt
extern const int ABN_player_play_from_mem;
#define ABW_player_play_from_mem             dsplayer[ABN_player_play_from_mem].wgt

/* 'files' Window link */
extern const int ABN_files;
#define ABW_files                            dsfiles[ABN_files].wgt
extern const int ABN_files_file_selector;
#define ABW_files_file_selector              dsfiles[ABN_files_file_selector].wgt
extern const int ABN_files_dir_name;
#define ABW_files_dir_name                   dsfiles[ABN_files_dir_name].wgt
extern const int ABN_files_load;
#define ABW_files_load                       dsfiles[ABN_files_load].wgt
extern const int ABN_files_save;
#define ABW_files_save                       dsfiles[ABN_files_save].wgt
extern const int ABN_files_secret_save;
#define ABW_files_secret_save                dsfiles[ABN_files_secret_save].wgt
extern const int ABN_files_secret_load;
#define ABW_files_secret_load                dsfiles[ABN_files_secret_load].wgt
extern const int ABN_files_button_done;
#define ABW_files_button_done                dsfiles[ABN_files_button_done].wgt

/* 'live' Window link */
extern const int ABN_live;
#define ABW_live                             dslive[ABN_live].wgt
extern const int ABN_base_output_img;
#define ABW_base_output_img                  dslive[ABN_base_output_img].wgt
extern const int ABN_base_browser_reset;
#define ABW_base_browser_reset               dslive[ABN_base_browser_reset].wgt
extern const int ABN_base_browser_next;
#define ABW_base_browser_next                dslive[ABN_base_browser_next].wgt
extern const int ABN_base_input_img;
#define ABW_base_input_img                   dslive[ABN_base_input_img].wgt
extern const int ABN_base_freeze_display;
#define ABW_base_freeze_display              dslive[ABN_base_freeze_display].wgt
extern const int ABN_base_action_no;
#define ABW_base_action_no                   dslive[ABN_base_action_no].wgt
extern const int ABN_base_displacement;
#define ABW_base_displacement                dslive[ABN_base_displacement].wgt
extern const int ABN_base_contact_frame;
#define ABW_base_contact_frame               dslive[ABN_base_contact_frame].wgt
extern const int ABN_base_unit_no;
#define ABW_base_unit_no                     dslive[ABN_base_unit_no].wgt
extern const int ABN_base_display_timer;
#define ABW_base_display_timer               dslive[ABN_base_display_timer].wgt

/* 'sequence' Window link */
extern const int ABN_sequence;
#define ABW_sequence                         dssequence[ABN_sequence].wgt
extern const int ABN_base_i1;
#define ABW_base_i1                          dssequence[ABN_base_i1].wgt
extern const int ABN_base_i2;
#define ABW_base_i2                          dssequence[ABN_base_i2].wgt
extern const int ABN_base_i3;
#define ABW_base_i3                          dssequence[ABN_base_i3].wgt
extern const int ABN_base_i4;
#define ABW_base_i4                          dssequence[ABN_base_i4].wgt
extern const int ABN_base_i5;
#define ABW_base_i5                          dssequence[ABN_base_i5].wgt
extern const int ABN_base_i6;
#define ABW_base_i6                          dssequence[ABN_base_i6].wgt
extern const int ABN_base_plot1;
#define ABW_base_plot1                       dssequence[ABN_base_plot1].wgt
extern const int ABN_base_plot2;
#define ABW_base_plot2                       dssequence[ABN_base_plot2].wgt
extern const int ABN_base_o2;
#define ABW_base_o2                          dssequence[ABN_base_o2].wgt
extern const int ABN_base_o3;
#define ABW_base_o3                          dssequence[ABN_base_o3].wgt
extern const int ABN_base_o4;
#define ABW_base_o4                          dssequence[ABN_base_o4].wgt
extern const int ABN_base_o5;
#define ABW_base_o5                          dssequence[ABN_base_o5].wgt
extern const int ABN_base_o6;
#define ABW_base_o6                          dssequence[ABN_base_o6].wgt
extern const int ABN_base_o1;
#define ABW_base_o1                          dssequence[ABN_base_o1].wgt
extern const int ABN_base_uptodate;
#define ABW_base_uptodate                    dssequence[ABN_base_uptodate].wgt
extern const int ABN_base_frame_stored;
#define ABW_base_frame_stored                dssequence[ABN_base_frame_stored].wgt
extern const int ABN_base_display_sequence;
#define ABW_base_display_sequence            dssequence[ABN_base_display_sequence].wgt
extern const int ABN_base_save_frames_toggle;
#define ABW_base_save_frames_toggle          dssequence[ABN_base_save_frames_toggle].wgt

/* 'stats' Window link */
extern const int ABN_stats;
#define ABW_stats                            dsstats[ABN_stats].wgt
extern const int ABN_stats_update;
#define ABW_stats_update                     dsstats[ABN_stats_update].wgt
extern const int ABN_stats_done;
#define ABW_stats_done                       dsstats[ABN_stats_done].wgt
extern const int ABN_stats_print;
#define ABW_stats_print                      dsstats[ABN_stats_print].wgt
extern const int ABN_stats_save;
#define ABW_stats_save                       dsstats[ABN_stats_save].wgt

#define AB_OPTIONS "s:x:y:h:w:S:"

#if defined(__cplusplus)
}
#endif

