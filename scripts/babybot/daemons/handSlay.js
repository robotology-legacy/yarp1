var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("handDisconnect.js", 7, true);

WshShell.Run ("slay -n cronus handcontrol.exe", 7, true);
