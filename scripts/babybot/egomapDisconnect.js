var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img !/egomap/i:img", 7, true);
WshShell.Run ("porter /egomap/o:img !/viewegomap/i:img", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/egomap/i:headposition", 7, true);

