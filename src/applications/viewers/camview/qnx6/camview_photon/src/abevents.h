/* Event header for application - AppBuilder 2.01  */

static const ApEventLink_t AbApplLinks[] = {
	{ 3, 0, 0L, 0L, 0L, &base, NULL, NULL, 0, NULL, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_base[] = {
	{ 5, 0, 0L, 0L, 0L, &filemenu, NULL, "base", 2007, NULL, 0, 1, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "RawZone", 2009, RawActivate, 0, 0, 0, 0, },
	{ 8, 3, 0L, 0L, 0L, NULL, NULL, "RawZone", 24000, (int(*)(PtWidget_t*,ApInfo_t*,PtCallbackInfo_t*)) dibujo, 0, 0, 0, 0, },
	{ 8, 3, 0L, 0L, 0L, NULL, NULL, "RawZone", 24003, (int(*)(PtWidget_t*,ApInfo_t*,PtCallbackInfo_t*)) raw_init, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Timer", 41002, RawActivate, 0, 0, 0, 0, },
	{ 5, 0, 0L, 0L, 0L, &filemenu, NULL, "base_file", 2007, NULL, 0, 1, 0, 0, },
	{ 5, 0, 0L, 0L, 0L, &imagemenu, NULL, "base_image", 2007, NULL, 0, 1, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_filemenu[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "file_name", 2009, save_image, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "file_exit", 2009, exit_viewer, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_imagemenu[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "image_frezee", 2009, image_freeze, 0, 0, 0, 0, },
	{ 0 }
	};

