var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("camerasDisconnect.js", 7, true);

WshShell.Run ("slay -n polifemo grabber-logpolar.exe", 7, true);
WshShell.Run ("slay -n cariddi camview.exe", 7, true);
