/* Link header for application - AppBuilder 2.01  */

extern ApContext_t AbContext;

ApWindowLink_t base = {
	"base.wgtw",
	&AbContext,
	AbLinks_base, 0, 7
	};

static ApItem_t ApItems_filemenu[ 3 ] = {
	{ 1, 1, 0, NULL, 0, "file_name", "Save Image", NULL },
	{ 1, 1, 0, NULL, 0, "file_exit", "Exit", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t filemenu = {
	"filemenu",
	"",
	NULL,
	NULL,
	-2,
	ApItems_filemenu,
	& AbContext,
	AbLinks_filemenu,
	6, 1, 2
	};

static ApItem_t ApItems_imagemenu[ 3 ] = {
	{ 1, 1, 0, NULL, 0, "image_frezee", "Freeze", NULL },
	{ 1, 1, 0, NULL, 0, "image_newratr", "New Rate", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t imagemenu = {
	"imagemenu",
	"",
	NULL,
	NULL,
	-2,
	ApItems_imagemenu,
	& AbContext,
	AbLinks_imagemenu,
	9, 1, 2
	};

