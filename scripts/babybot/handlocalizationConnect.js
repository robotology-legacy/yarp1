var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img /handlocalization/i:img", 7, false);
WshShell.Run ("porter /handlocalization/o:img /view006/i:img", 7, false);

WshShell.Run ("porter /handlocalization/segmentation/o:img /view007/i:img", 7, false);
// WshShell.Run ("porter /handlocalization/o:pixel /vect/i", 7, false);
WshShell.Run ("porter /handcontrol/o:status /handlocalization/i:hand", 7, false);
WshShell.Run ("porter /armcontrol/o:status /handlocalization/i:hand", 7, false);
WshShell.Run ("porter /repeater/o /handlocalization/behavior/i", 7, false);

WshShell.Run ("porter /handlocalization/segmentation/o:img /handtracker/segmentation/i:img", 7, false);

WshShell.Run ("porter /left/o:img /handtracker/i:img", 7, false);
WshShell.Run ("porter /handtracker/segmentation/o:img /view008/i:img", 7, false);

// porter /view006/o:point /handlocalization/i:point







