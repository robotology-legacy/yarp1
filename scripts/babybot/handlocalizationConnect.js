var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img /handlocalization/i:img", 7, true);
WshShell.Run ("porter /handlocalization/o:img /view006/i:img", 7, true);

WshShell.Run ("porter /handlocalization/segmentation/o:img /view007/i:img", 7, true);
// WshShell.Run ("porter /handlocalization/o:pixel /vect/i", 7, true);
WshShell.Run ("porter /armcontrol/o:status /handlocalization/i:motor", 7, true);
WshShell.Run ("porter /armcontrol/o:status /handtracker/i:arm", 7, true);
WshShell.Run ("porter /repeater/o /handlocalization/behavior/i", 7, true);

WshShell.Run ("porter /handlocalization/segmentation/o:img /handtracker/segmentation/i:img", 7, true);

WshShell.Run ("porter /left/o:img /handtracker/i:img", 7, true);
WshShell.Run ("porter /handtracker/segmentation/o:img /view008/i:img", 7, true);
WshShell.Run ("porter /handtracker/backprojection/o:img /view009/i:img", 7, true);
WshShell.Run ("porter /handlocalization/segmentation/o:armdata /handtracker/segmentation/i:armdata", 7, true);


// porter /view006/o:point /handlocalization/i:point




