var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("porter /armcontrol/o:status !/handtracker/arm/i", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/handtracker/head/i", 7, true);

WshShell.Run ("porter /handlocalization/segmentation/o:img !/view007/i:img", 7, true);
WshShell.Run ("porter /handlocalization/segmentation/o:img !/handtracker/segmentation/i:img", 7, true);

WshShell.Run ("porter /left/o:img !/handtracker/i:img", 7, true);
WshShell.Run ("porter /handtracker/segmentation/o:img !/view008/i:img", 7, true);
WshShell.Run ("porter /handtracker/backprojection/o:img !/view009/i:img", 7, true);

WshShell.Run ("slay -n leto handcolortracke", 7, true);
WshShell.Run ("slay -n leto camview.exe", 7, true);

WshShell.Run ("slay -n calliope remotelearn.exe", 7, true);