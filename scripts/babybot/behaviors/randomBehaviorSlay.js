var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("randomBehaviorDisconnect.js", 7, true);

WshShell.Run ("slay -n rhea behaviorrnd.exe", 7, true);
