var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("handtrackerDisconnect.js", 7, true);

WshShell.Run ("slay -n hades handcolortracker.exe", 7, true);
WshShell.Run ("slay -n hades camview.exe", 7, true);
//WshShell.Run ("slay -n calliope remotelearn.exe", 7, true);