var WshShell = WScript.CreateObject("WScript.Shell");
// WshShell.Run ("behaviorrnd --shake", 1, true);
WshShell.Run ("on -d -n pento behaviorrnd", 1, true);