/* Event header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

const ApEventLink_t appl_links[] = {
	{ 3, 0, 0L, 0L, 0L, &base, base_links, NULL, 0, NULL, 0, 0, 0, 0, dsbase },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

ApEventLink_t internal_links[] = {
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t base_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "exit_button", 2009, exit_callback, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "smaller_button", 2009, resize_smaller, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "bigger_button", 2009, resize_bigger, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "freeze_button", 8001, freeze, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "refresh_timer", 41002, viewer_draw, 0, 0, 0, 0, NULL },
	{ 3, 0, 0L, 0L, 0L, &file_selection_window, file_selection_window_links, "set_file_button", 2009, NULL, 0, 0, 0, 0, dsfile_selection_window },
	{ 3, 0, 0L, 0L, 0L, &progress_bar, progress_bar_links, "save_button", 2009, NULL, 0, 0, 0, 0, dsprogress_bar },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "save_button", 2009, save, 0, 0, 0, 0, NULL },
	{ 3, 0, 0L, 0L, 0L, &progress_bar, progress_bar_links, "load_button", 2009, NULL, 0, 0, 0, 0, dsprogress_bar },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "load_button", 2009, load, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "trajectory_combo", 31020, trajectory_color, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "raw_label", 1012, initial_size, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t file_selection_window_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "file_selector", 47007, set_selection, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "filter_input", 2009, change_filter, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "filename_input", 2009, change_selection, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "filename_input", 1012, realized, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "filename_input", 1013, unrealized, 0, 0, 0, 0, NULL },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "close_file_button", 2009, NULL, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t progress_bar_links[] = {
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};


#if defined(__cplusplus)
}
#endif

