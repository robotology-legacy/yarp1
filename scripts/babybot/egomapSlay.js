var WshShell = WScript.CreateObject("WScript.Shell");


/// slay
WshShell.Run ("slay -n leto egomap.exe", 7, true);
WshShell.Run ("slay -n leto camview.exe ", 7, true);
