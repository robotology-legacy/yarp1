var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("mirrorCaptureDisconnect.js", 7, true);

WshShell.Run ("slay -n atlas graspcapture.exe", 7, true);
WshShell.Run ("slay -n atlas mirrorCollector.exe", 7, true);
