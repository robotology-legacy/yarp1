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
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base", 18023, initialize, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonTarget1", 2009, do_target1, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonTarget2", 2009, do_target2, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonTarget3", 2009, do_target3, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonTarget4", 2009, do_target4, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonTarget5", 2009, do_target5, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonRevert1", 2009, do_revert1, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonRevert2", 2009, do_revert2, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonRevert3", 2009, do_revert3, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonRevert4", 2009, do_revert4, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonRevert5", 2009, do_revert5, 0, 0, 0, 0, NULL },
	{ 8, 1, 0L, 0L, 2L, NULL, NULL, "PtRectTarget", 1011, go_point, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonReset", 2009, do_reset, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonAppear1", 2009, do_appear1, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonAppear2", 2009, do_appear2, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonAppear3", 2009, do_appear3, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonAppear4", 2009, do_appear4, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButtonAppear5", 2009, do_appear5, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};

const char ApOptions[] = AB_OPTIONS;

#if defined(__cplusplus)
}
#endif

