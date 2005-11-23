
extern int flipper_main(int argc, char *argv[]);

extern void flipper_end();

extern void flipper_apply(YARPImageOf<YarpPixelBGR>& src,
					   YARPImageOf<YarpPixelMono>& mask,
					   YARPImageOf<YarpPixelBGR>& dest, 
					   int ignore=0);

extern void flipper_apply(YARPImageOf<YarpPixelBGR>& src,
				   YARPImageOf<YarpPixelBGR>& dest,
				   int& xx, int& yy, int& ff);
