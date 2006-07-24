var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("camerasDisconnect.js", 7, true);

WshShell.Run ("slay -n polifemo grabber-logpolar.exe", 7, true);
WshShell.Run ("slay -n cariddi cam_camview.exe", 7, true);
WshShell.Run ("cmd /C \"del Y:\\bin\\winnt\\cam_camview.exe\"", 7, true);
