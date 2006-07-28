var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n rhea behaviorrnd --shake", 7, true);
