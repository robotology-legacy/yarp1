var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img /tracker/i:img", 7, true);
WshShell.Run ("porter /headcontrol/position/o /tracker/i:headposition", 7, true);

// disconnect from frameg.
WshShell.Run ("porter /left/o:img !/view003/i:img", 7, true);

// reattach to display
WshShell.Run ("porter /tracker/o:img /view003/i:img", 7, true);
