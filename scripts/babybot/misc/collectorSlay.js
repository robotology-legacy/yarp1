var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("collectorDisconnect.js", 7, true);
WshShell.Run ("slay -n circe collectpoints.exe", 7, true);
