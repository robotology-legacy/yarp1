var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n circe collectpoints.exe", 7, true);
