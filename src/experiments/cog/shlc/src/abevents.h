/* Event header for application - AppBuilder 1.14C */

#if defined(__cplusplus)
extern "C" {
#endif

const ApEventLink_t appl_links[] = {
	{ 3, 0, 0L, 0L, 0L, &base, base_links, NULL, 0, NULL, 0, 0, 0, 0, dsbase },
	{ 3, 0, 0L, 0L, 0L, &player, player_links, NULL, 0, NULL, 0, 0, 0, 0, dsplayer },
	{ 3, 0, 0L, 0L, 0L, &live, live_links, NULL, 0, NULL, 0, 0, 0, 0, dslive },
	{ 3, 0, 0L, 0L, 0L, &sequence, sequence_links, NULL, 0, NULL, 0, 0, 0, 0, dssequence },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

ApEventLink_t internal_links[] = {
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t base_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base", 18024, quit, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_quit", 2009, quit, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_connect", 2009, connect, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_enable_recording", 2009, enable_recording, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_process", 2009, process_sequence, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_init_of", 2009, init_opticflow, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_clear_of", 2009, disable_opticflow, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_secret", 2009, enable_secret, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_disablesecret", 2009, disable_secret, 0, 0, 0, 0, NULL },
	{ 3, 0, 0L, 0L, 0L, &files, files_links, "base_open_files_w", 2009, NULL, 0, 0, 0, 0, dsfiles },
	{ 3, 0, 0L, 0L, 0L, &stats, stats_links, "base_canonical_stat", 2009, NULL, 0, 0, 0, 0, dsstats },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_find", 2009, find_object, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t player_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "player_rewind_memory", 2009, rewind_memory, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "player_back_memory", 2009, back_memory, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "player_frorward_memory", 2009, forward_memory, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "player_forward_10", 2009, forward_memory_10, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "player_play_from_mem", 2009, toggle_play_from_mem, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t files_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_file_selector", 47007, change_dir_name, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_dir_name", 2009, change_dir_name, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_dir_name", 1012, realized_dir_name, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_dir_name", 1013, unrealized_dir_name, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_load", 2009, load_data, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_save", 2009, save_data, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_secret_save", 2009, save_canonical, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "files_secret_load", 2009, load_canonical, 0, 0, 0, 0, NULL },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "files_button_done", 2009, NULL, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t live_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_browser_reset", 2009, browser_init_cb, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_browser_next", 2009, browser_next_cb, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_input_img", 1012, realize_display, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_input_img", 1013, unrealize_display, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_freeze_display", 2009, toggle_freeze_display, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_display_timer", 41002, display_timer_activate, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t sequence_links[] = {
	{ 8, 3, 0L, 0L, 0L, NULL, NULL, "base_plot1", 24000, (int(*)(PtWidget_t*,ApInfo_t*,PtCallbackInfo_t*)) draw_plot1, 0, 0, 0, 0, NULL },
	{ 8, 3, 0L, 0L, 0L, NULL, NULL, "base_plot2", 24000, (int(*)(PtWidget_t*,ApInfo_t*,PtCallbackInfo_t*)) draw_plot2, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base_display_sequence", 2009, display_sequence, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t stats_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "stats_update", 2009, update, 0, 0, 0, 0, NULL },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "stats_done", 2009, NULL, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "stats_print", 2009, print_data, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "stats_save", 2009, save, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};


#if defined(__cplusplus)
}
#endif

