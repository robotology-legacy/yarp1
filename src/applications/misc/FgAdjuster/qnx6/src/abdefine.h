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

#define AbGetABW( n ) ( AbWidgets[ n ].wgt )

#define AB_OPTIONS "s:x:y:h:w:S:"
