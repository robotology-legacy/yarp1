var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first
WshShell.Run ("porter /left/o:img !/egomap/i:img", 7, true);
WshShell.Run ("porter /egomap/o:img !/viewegomap/i:img", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/egomap/i:headposition", 7, true);

/// slay
WshShell.Run ("slay -n calliope egomap.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe ", 7, true);
