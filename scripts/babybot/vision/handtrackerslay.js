var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("handtrackerDisconnect.js", 7, true);

WshShell.Run ("slay -n oceanus handcolortracke", 7, true);
WshShell.Run ("slay -n calliope remotelearn.exe", 7, true);