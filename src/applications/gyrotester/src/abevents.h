/* Event header for application - AppBuilder 1.14C */

const ApEventLink_t appl_links[] = {
	{ 3, 0, 0L, 0L, 0L, &base, base_links, NULL, 0, NULL, 0, 0, 0, 0, dsbase },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

ApEventLink_t internal_links[] = {
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const ApEventLink_t base_links[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "global_timer", 41002, timer_activate, 0, 0, 0, 0, NULL },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "quit_button", 2009, NULL, 0, 0, 0, 0, NULL },
	{ 8, 3, 0L, 0L, 0L, NULL, NULL, "rawcube", 24000, (int(*)(PtWidget_t*,ApInfo_t*,PtCallbackInfo_t*)) draw_cube, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "boresight_button", 2009, boresight, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "unboresight_button", 2009, unboresight, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

