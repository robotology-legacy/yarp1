var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("egoMap2Disconnect.js", 7, true);
WshShell.Run ("slay -n leto egomap.exe", 7, true);
WshShell.Run ("slay -n leto camview.exe", 7, true);