var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n proteus forcesensor.exe", 7, false);