var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img !/tracker/i:img", 7, true);
WshShell.Run ("porter /tracker/o:img !/view0010/i:img", 7, true);
WshShell.Run ("porter /tracker/o:vect !/viewer/tracker/i", 7, true);
WshShell.Run ("slay -n euterpe tracker.exe", 7, true);
WshShell.Run ("slay -n euterpe vectviewer.exe ", 7, true);
