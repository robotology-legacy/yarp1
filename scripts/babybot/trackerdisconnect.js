var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first
WshShell.Run ("porter /left/o:img !/tracker/i:img", 7, true);
WshShell.Run ("porter /tracker/o:img !/view003/i:img", 7, true);
WshShell.Run ("porter /tracker/o:vect !/viewer/tracker/i", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/tracker/i:headposition", 7, true);