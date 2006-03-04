var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("BodyMapDisconnect.js", 7, true);

WshShell.Run ("slay -n eolo BodyMap.exe", 7, true);
WshShell.Run ("slay -n eolo learner.exe", 7, true);
WshShell.Run ("slay -n atlas mirrorCollector.exe", 7, true);
