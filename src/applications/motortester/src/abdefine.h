/* Define header for application - AppBuilder 1.14C */

/* Internal Module Links */

/* 'base' Window link */
extern const int ABN_base;
#define ABW_base                             dsbase[ABN_base].wgt
extern const int ABN_velocity_input;
#define ABW_velocity_input                   dsbase[ABN_velocity_input].wgt
extern const int ABN_acceleration_input;
#define ABW_acceleration_input               dsbase[ABN_acceleration_input].wgt
extern const int ABN_goal_position_2;
#define ABW_goal_position_2                  dsbase[ABN_goal_position_2].wgt
extern const int ABN_goal_position_1;
#define ABW_goal_position_1                  dsbase[ABN_goal_position_1].wgt
extern const int ABN_repeat_button;
#define ABW_repeat_button                    dsbase[ABN_repeat_button].wgt
extern const int ABN_goto_goal_2;
#define ABW_goto_goal_2                      dsbase[ABN_goto_goal_2].wgt
extern const int ABN_goto_goal_1;
#define ABW_goto_goal_1                      dsbase[ABN_goto_goal_1].wgt
extern const int ABN_zero_button;
#define ABW_zero_button                      dsbase[ABN_zero_button].wgt
extern const int ABN_position_trend;
#define ABW_position_trend                   dsbase[ABN_position_trend].wgt
extern const int ABN_title_label;
#define ABW_title_label                      dsbase[ABN_title_label].wgt
extern const int ABN_global_timer;
#define ABW_global_timer                     dsbase[ABN_global_timer].wgt
extern const int ABN_quit_button;
#define ABW_quit_button                      dsbase[ABN_quit_button].wgt
extern const int ABN_reset_button;
#define ABW_reset_button                     dsbase[ABN_reset_button].wgt
extern const int ABN_in_motion_light;
#define ABW_in_motion_light                  dsbase[ABN_in_motion_light].wgt
extern const int ABN_in_position_light;
#define ABW_in_position_light                dsbase[ABN_in_position_light].wgt
extern const int ABN_in_sequence_light;
#define ABW_in_sequence_light                dsbase[ABN_in_sequence_light].wgt
extern const int ABN_position_tag_label;
#define ABW_position_tag_label               dsbase[ABN_position_tag_label].wgt
extern const int ABN_position_label;
#define ABW_position_label                   dsbase[ABN_position_label].wgt
extern const int ABN_axis_selector;
#define ABW_axis_selector                    dsbase[ABN_axis_selector].wgt
extern const int ABN_axis_state_label;
#define ABW_axis_state_label                 dsbase[ABN_axis_state_label].wgt
extern const int ABN_p_gain;
#define ABW_p_gain                           dsbase[ABN_p_gain].wgt
extern const int ABN_i_gain;
#define ABW_i_gain                           dsbase[ABN_i_gain].wgt
extern const int ABN_d_gain;
#define ABW_d_gain                           dsbase[ABN_d_gain].wgt
extern const int ABN_accel_ff;
#define ABW_accel_ff                         dsbase[ABN_accel_ff].wgt
extern const int ABN_vel_ff;
#define ABW_vel_ff                           dsbase[ABN_vel_ff].wgt
extern const int ABN_dac_limit;
#define ABW_dac_limit                        dsbase[ABN_dac_limit].wgt
extern const int ABN_i_limit;
#define ABW_i_limit                          dsbase[ABN_i_limit].wgt
extern const int ABN_friction_ff;
#define ABW_friction_ff                      dsbase[ABN_friction_ff].wgt
extern const int ABN_offset;
#define ABW_offset                           dsbase[ABN_offset].wgt
extern const int ABN_shift;
#define ABW_shift                            dsbase[ABN_shift].wgt
extern const int ABN_trigger_switch;
#define ABW_trigger_switch                   dsbase[ABN_trigger_switch].wgt
extern const int ABN_sample_rate;
#define ABW_sample_rate                      dsbase[ABN_sample_rate].wgt
extern const int ABN_trend_min_value;
#define ABW_trend_min_value                  dsbase[ABN_trend_min_value].wgt
extern const int ABN_trend_max_value;
#define ABW_trend_max_value                  dsbase[ABN_trend_max_value].wgt
extern const int ABN_calibrate_button;
#define ABW_calibrate_button                 dsbase[ABN_calibrate_button].wgt
extern const int ABN_link_master_selector;
#define ABW_link_master_selector             dsbase[ABN_link_master_selector].wgt
extern const int ABN_link_slave_selector;
#define ABW_link_slave_selector              dsbase[ABN_link_slave_selector].wgt
extern const int ABN_link_ratio;
#define ABW_link_ratio                       dsbase[ABN_link_ratio].wgt
extern const int ABN_link_enable_button;
#define ABW_link_enable_button               dsbase[ABN_link_enable_button].wgt
extern const int ABN_save_filter_button;
#define ABW_save_filter_button               dsbase[ABN_save_filter_button].wgt
extern const int ABN_upload_file_button;
#define ABW_upload_file_button               dsbase[ABN_upload_file_button].wgt
extern const int ABN_download_file_button;
#define ABW_download_file_button             dsbase[ABN_download_file_button].wgt
extern const int ABN_soft_shutdown_button;
#define ABW_soft_shutdown_button             dsbase[ABN_soft_shutdown_button].wgt

/* 'file_selector_window' Window link */
extern const int ABN_file_selector_window;
#define ABW_file_selector_window             dsfile_selector_window[ABN_file_selector_window].wgt
extern const int ABN_file_tree;
#define ABW_file_tree                        dsfile_selector_window[ABN_file_tree].wgt
extern const int ABN_file_cancel_button;
#define ABW_file_cancel_button               dsfile_selector_window[ABN_file_cancel_button].wgt
extern const int ABN_filter_input;
#define ABW_filter_input                     dsfile_selector_window[ABN_filter_input].wgt
extern const int ABN_selection_input;
#define ABW_selection_input                  dsfile_selector_window[ABN_selection_input].wgt
extern const int ABN_file_accept_button;
#define ABW_file_accept_button               dsfile_selector_window[ABN_file_accept_button].wgt

#define AB_OPTIONS "s:x:y:h:w:S:"
