/* Event header for application - AppBuilder 1.14C */

const ApEventLink_t appl_links[] = {
	{ 3, 0, 0L, 0L, 0L, &base, base_links, NULL, 0, NULL, 0, 0, 0, 0, dsbase },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

ApEventLink_t internal_links[] = {
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t base_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base", 18023, initialize, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "repeat_button", 8001, enable_repeat, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "goto_goal_2", 2009, goto_goal, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "goto_goal_1", 2009, goto_goal, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "zero_button", 2009, zero_encoder, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "global_timer", 41002, update_position_trend, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "quit_button", 2009, quit, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "reset_button", 2009, reset, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "axis_selector", 31020, axis_change, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "p_gain", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "i_gain", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "d_gain", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "accel_ff", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "vel_ff", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "dac_limit", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "i_limit", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "friction_ff", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "offset", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "shift", 53015, change_gains, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "trigger_switch", 8001, enable_trigger, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "sample_rate", 53015, change_sample_rate, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "trend_min_value", 53015, change_trend_min, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "trend_max_value", 53015, change_trend_max, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "calibrate_button", 2009, calibrate, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "link_enable_button", 8001, enable_link, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "save_filter_button", 2009, save_filter, 0, 0, 0, 0, NULL },
	{ 3, 0, 0L, 0L, 0L, &file_selector_window, file_selector_window_links, "upload_file_button", 2009, file_setup_upload, 0, 11, 0, 0, dsfile_selector_window },
	{ 3, 0, 0L, 0L, 0L, &file_selector_window, file_selector_window_links, "download_file_button", 2009, file_setup_download, 0, 11, 0, 0, dsfile_selector_window },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "soft_shutdown_button", 2009, soft_shutdown, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t file_selector_window_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "file_tree", 47007, file_set_selection, 0, 0, 0, 0, NULL },
	{ 10, 0, 0L, 0L, 0L, NULL, NULL, "file_cancel_button", 2009, NULL, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "filter_input", 2009, file_change_filter, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "selection_input", 2009, file_change_selection, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "file_accept_button", 2009, file_accept, 0, 0, 0, 0, NULL },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "file_accept_button", 2009, NULL, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

