/* Define header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

/* Internal Module Links */

/* 'base' Window link */
extern const int ABN_base;
#define ABW_base                             dsbase[ABN_base].wgt
extern const int ABN_command_group;
#define ABW_command_group                    dsbase[ABN_command_group].wgt
extern const int ABN_exit_button;
#define ABW_exit_button                      dsbase[ABN_exit_button].wgt
extern const int ABN_smaller_button;
#define ABW_smaller_button                   dsbase[ABN_smaller_button].wgt
extern const int ABN_bigger_button;
#define ABW_bigger_button                    dsbase[ABN_bigger_button].wgt
extern const int ABN_freeze_button;
#define ABW_freeze_button                    dsbase[ABN_freeze_button].wgt
extern const int ABN_target_label;
#define ABW_target_label                     dsbase[ABN_target_label].wgt
extern const int ABN_refresh_timer;
#define ABW_refresh_timer                    dsbase[ABN_refresh_timer].wgt
extern const int ABN_set_file_button;
#define ABW_set_file_button                  dsbase[ABN_set_file_button].wgt
extern const int ABN_save_button;
#define ABW_save_button                      dsbase[ABN_save_button].wgt
extern const int ABN_load_button;
#define ABW_load_button                      dsbase[ABN_load_button].wgt
extern const int ABN_trajectory_combo;
#define ABW_trajectory_combo                 dsbase[ABN_trajectory_combo].wgt
extern const int ABN_raw_label;
#define ABW_raw_label                        dsbase[ABN_raw_label].wgt

/* 'file_selection_window' Window link */
extern const int ABN_file_selection_window;
#define ABW_file_selection_window            dsfile_selection_window[ABN_file_selection_window].wgt
extern const int ABN_file_selector;
#define ABW_file_selector                    dsfile_selection_window[ABN_file_selector].wgt
extern const int ABN_filter_input;
#define ABW_filter_input                     dsfile_selection_window[ABN_filter_input].wgt
extern const int ABN_filename_input;
#define ABW_filename_input                   dsfile_selection_window[ABN_filename_input].wgt
extern const int ABN_close_file_button;
#define ABW_close_file_button                dsfile_selection_window[ABN_close_file_button].wgt

/* 'progress_bar' Window link */
extern const int ABN_progress_bar;
#define ABW_progress_bar                     dsprogress_bar[ABN_progress_bar].wgt
extern const int ABN_progress_chart;
#define ABW_progress_chart                   dsprogress_bar[ABN_progress_chart].wgt
extern const int ABN_progress_label;
#define ABW_progress_label                   dsprogress_bar[ABN_progress_label].wgt

#define AB_OPTIONS "s:x:y:h:w:S:"

#if defined(__cplusplus)
}
#endif

