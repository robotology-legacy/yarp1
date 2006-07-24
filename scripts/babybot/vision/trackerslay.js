var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("trackerDisconnect.js", 7, true);

WshShell.Run ("slay -n cariddi tracker.exe", 7, true);
WshShell.Run ("slay -n cariddi trk_camview.exe", 7, true);
WshShell.Run ("cmd /C \"del Y:\\bin\\winnt\\trk_camview.exe\"", 7, true);
