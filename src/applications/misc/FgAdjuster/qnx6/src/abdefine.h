/* Define header for application - AppBuilder 2.01  */

/* 'base' Window link */
extern const int ABN_base;
#define ABW_base                             AbGetABW( ABN_base )
extern const int ABN_slider_bright;
#define ABW_slider_bright                    AbGetABW( ABN_slider_bright )
extern const int ABN_slider_hue;
#define ABW_slider_hue                       AbGetABW( ABN_slider_hue )
extern const int ABN_slider_contrast;
#define ABW_slider_contrast                  AbGetABW( ABN_slider_contrast )
extern const int ABN_slider_satu;
#define ABW_slider_satu                      AbGetABW( ABN_slider_satu )
extern const int ABN_slider_satv;
#define ABW_slider_satv                      AbGetABW( ABN_slider_satv )
extern const int ABN_text_bright;
#define ABW_text_bright                      AbGetABW( ABN_text_bright )
extern const int ABN_text_hue;
#define ABW_text_hue                         AbGetABW( ABN_text_hue )
extern const int ABN_text_contrast;
#define ABW_text_contrast                    AbGetABW( ABN_text_contrast )
extern const int ABN_text_satu;
#define ABW_text_satu                        AbGetABW( ABN_text_satu )
extern const int ABN_text_satv;
#define ABW_text_satv                        AbGetABW( ABN_text_satv )
extern const int ABN_togglebuttonLNotch;
#define ABW_togglebuttonLNotch               AbGetABW( ABN_togglebuttonLNotch )
extern const int ABN_togglebuttonLDec;
#define ABW_togglebuttonLDec                 AbGetABW( ABN_togglebuttonLDec )
extern const int ABN_togglebuttonCrush;
#define ABW_togglebuttonCrush                AbGetABW( ABN_togglebuttonCrush )
extern const int ABN_togglebuttonPeak;
#define ABW_togglebuttonPeak                 AbGetABW( ABN_togglebuttonPeak )
extern const int ABN_togglebuttonCagc;
#define ABW_togglebuttonCagc                 AbGetABW( ABN_togglebuttonCagc )
extern const int ABN_togglebuttonCkill;
#define ABW_togglebuttonCkill                AbGetABW( ABN_togglebuttonCkill )
extern const int ABN_togglebuttonCrushRange;
#define ABW_togglebuttonCrushRange           AbGetABW( ABN_togglebuttonCrushRange )
extern const int ABN_togglebuttonYsleep;
#define ABW_togglebuttonYsleep               AbGetABW( ABN_togglebuttonYsleep )
extern const int ABN_togglebuttonCsleep;
#define ABW_togglebuttonCsleep               AbGetABW( ABN_togglebuttonCsleep )
extern const int ABN_togglebuttonGamma;
#define ABW_togglebuttonGamma                AbGetABW( ABN_togglebuttonGamma )
extern const int ABN_togglebuttonDithFrame;
#define ABW_togglebuttonDithFrame            AbGetABW( ABN_togglebuttonDithFrame )

#define AbGetABW( n ) ( AbWidgets[ n ].wgt )

#define AB_OPTIONS "s:x:y:h:w:S:"
