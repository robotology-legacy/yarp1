/* Define header for application - AppBuilder 2.01  */

/* 'base' Window link */
extern const int ABN_base;
#define ABW_base                             AbGetABW( ABN_base )
extern const int ABN_m_OSContainer;
#define ABW_m_OSContainer                    AbGetABW( ABN_m_OSContainer )
extern const int ABN_m_Group;
#define ABW_m_Group                          AbGetABW( ABN_m_Group )
extern const int ABN_RawZone;
#define ABW_RawZone                          AbGetABW( ABN_RawZone )
extern const int ABN_name_text;
#define ABW_name_text                        AbGetABW( ABN_name_text )
extern const int ABN_Timer;
#define ABW_Timer                            AbGetABW( ABN_Timer )
extern const int ABN_base_MenuBar;
#define ABW_base_MenuBar                     AbGetABW( ABN_base_MenuBar )
extern const int ABN_base_file;
#define ABW_base_file                        AbGetABW( ABN_base_file )
extern const int ABN_base_image;
#define ABW_base_image                       AbGetABW( ABN_base_image )

#define AbGetABW( n ) ( AbWidgets[ n ].wgt )

/* 'filemenu' Menu link */
extern const int ABN_filemenu;
extern const int ABN_file_name;
extern const int ABN_file_exit;

/* 'imagemenu' Menu link */
extern const int ABN_imagemenu;
extern const int ABN_image_frezee;
extern const int ABN_image_newratr;

#define AB_OPTIONS "s:"
