var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first
WshShell.Run ("porter /left/o:img !/vergence/i:left", 7, true);
WshShell.Run ("porter /right/o:img !/vergence/i:right", 7, true);
WshShell.Run ("porter /vergence/o:histo !/viewvergence/i:img", 7, true);

/// slay
WshShell.Run ("slay -n calliope vergence.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe ", 7, true);
