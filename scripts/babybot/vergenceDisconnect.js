var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img !/vergence/i:left", 7, true);
WshShell.Run ("porter /right/o:img !/vergence/i:right", 7, true);
WshShell.Run ("porter /vergence/o:histo !/viewvergence/i:img", 7, true);

