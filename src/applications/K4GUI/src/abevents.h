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
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend1MaxValue", 53015, Trend1MaxChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend1MinValue", 53015, Trend1MinChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AxisSelector1", 31020, Trend1AxisChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "MEID_Reset_Button", 2009, MEIDreset, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PIC_Reset_Button", 2009, ResetPIC, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "QuitButton", 2009, quit, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AxisSelector", 31020, axischange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ZeroEncoderButton", 2009, zero_encoder, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "MoveButton", 2009, MoveToPos, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "TurnOnAllButton", 2009, TurnOnAll, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "TurnOffAllButton", 2009, TurnOffAll, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis15Slider", 22017, axis15slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis15OnOff", 2009, Axis15Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis16Slider", 22017, axis16slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis16OnOff", 2009, Axis16Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis17Slider", 22017, axis17slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis17OnOff", 2009, Axis17Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis18Slider", 22017, axis18slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis18OnOff", 2009, Axis18Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis19Slider", 22017, axis19slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis19OnOff", 2009, Axis19Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis20Slider", 22017, axis20slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis20OnOff", 2009, Axis20Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis21Slider", 22017, axis21slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis21OnOff", 2009, Axis21Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis22Slider", 22017, axis22slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis22OnOff", 2009, Axis22Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis23Slider", 22017, axis23slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis23OnOff", 2009, Axis23Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis24Slider", 22017, axis24slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis24OnOff", 2009, Axis24Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis25Slider", 22017, axis25slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis25OnOff", 2009, Axis25Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis26Slider", 22017, axis26slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis26OnOff", 2009, Axis26Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis27Slider", 22017, axis27slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis27OnOff", 2009, Axis27Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis28Slider", 22017, axis28slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis28OnOff", 2009, Axis28Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis29Slider", 22017, axis29slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis29OnOff", 2009, Axis29Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis0Slider", 22017, axis0slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis0OnOff", 2009, Axis0Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis1Slider", 22017, axis1slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis1OnOff", 2009, Axis1Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis2Slider", 22017, axis2slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis2OnOff", 2009, Axis2Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis3Slider", 22017, axis3slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis3OnOff", 2009, Axis3Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis4Slider", 22017, axis4slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis4OnOff", 2009, Axis4Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis5Slider", 22017, axis5slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis5OnOff", 2009, Axis5Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis6Slider", 22017, axis6slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis6OnOff", 2009, Axis6Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis7Slider", 22017, axis7slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis7OnOff", 2009, Axis7Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis8Slider", 22017, axis8slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis8OnOff", 2009, Axis8Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis9Slider", 22017, axis9slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis9OnOff", 2009, Axis9Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis10Slider", 22017, axis10slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis10OnOff", 2009, Axis10Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis11Slider", 22017, axis11slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis11OnOff", 2009, Axis11Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis12Slider", 22017, axis12slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis12OnOff", 2009, Axis12Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis13Slider", 22017, axis13slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis13OnOff", 2009, Axis13Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis14Slider", 22017, axis14slider_move, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Axis14OnOff", 2009, Axis14Toggle, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend3MaxValue", 53015, Trend3MaxChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend3MinValue", 53015, Trend3MinChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AxisSelector3", 31020, Trend3AxisChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend4MaxValue", 53015, Trend4MaxChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend4MinValue", 53015, Trend4MinChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AxisSelector4", 31020, Trend4AxisChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "EncoderTimerAll", 41002, TimerActivate, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend5", 1012, Realized, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend5MaxValue", 53015, Trend5MaxChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend5MinValue", 53015, Trend5MinChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend2MaxValue", 53015, Trend2MaxChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Trend2MinValue", 53015, Trend2MinChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AxisSelector2", 31020, Trend2AxisChange, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "SetVelocityButton", 2009, SetVelocity, 0, 0, 0, 0, NULL },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "CalibrateAllButton", 2009, CalibrateAll, 0, 0, 0, 0, NULL },
	{ 0, 0, 0L, 0L, 0L, NULL, NULL, NULL, 0, NULL, 0, 0, 0, 0, NULL }
	};


#if defined(__cplusplus)
}
#endif

