var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("paxisDisconnect.js", 7, true);

WshShell.Run ("slay -n circe paxis.exe", 7, true);
