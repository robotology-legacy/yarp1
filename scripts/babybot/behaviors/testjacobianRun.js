var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n circe testjacobian.exe", 7, true);
