var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first
WshShell.Run ("porter /left/o:img !/tracker/i:img", 7, true);
WshShell.Run ("porter /tracker/o:img !/view003/i:img", 7, true);
WshShell.Run ("porter /tracker/o:vect !/viewer/tracker/i", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/tracker/i:headposition", 7, true);

/// reattach std display.
WshShell.Run ("porter /left/o:img /view003/i:img", 7, true);

/// slay
WshShell.Run ("slay -n euterpe tracker.exe", 7, true);
WshShell.Run ("slay -n euterpe vectviewer.exe ", 7, true);
