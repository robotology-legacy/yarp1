var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("headDisconnect.js", 7, true);

WshShell.Run ("slay -n thaumas headcontrol.exe", 7, true);
