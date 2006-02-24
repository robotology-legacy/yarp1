var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("armDisconnect.js", 7, true);

WshShell.Run ("slay -n proteus armcontrol.exe", 7, true);
