var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img /egomap/image/i", 7, true);
WshShell.Run ("porter /egomap/image/o /egomapview/i:img", 7, true);
WshShell.Run ("porter /headcontrol/position/o /egomap/head/i", 7, true);

