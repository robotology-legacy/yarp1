var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img /tracker/i:img", 7, true);

// disconnect from frameg.
WshShell.Run ("porter /left/o:img !/view003/i:img", 7, true);

WshShell.Run ("porter /tracker/o:img /view003/i:img", 7, true);
//WshShell.Run ("porter /tracker/o:vect /viewer/tracker/i", 7, true);
