var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("reachingDisconnect.js", 7, true);
WshShell.Run ("slay -d -n proteus reaching.exe", 7, true);
// WshShell.Run ("slay -d -n pento armtrigger.exe", 7, true);