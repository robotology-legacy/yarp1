var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n eolo \"porter /left/o:img !/egomap/i:img\"", 7, true);
WshShell.Run ("on -d -n eolo \"porter /egomap/o:img !/viewegomap/i:img\"", 7, true);
WshShell.Run ("on -d -n eolo \"porter /headcontrol/position/o !/egomap/i:headposition\"", 7, true);

