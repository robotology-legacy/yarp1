var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("handtrackerSVMDisconnect.js", 7, true);

WshShell.Run ("slay -n hades handcolortrackersvm.exe", 7, true);
WshShell.Run ("slay -n hades htk_camview.exe", 7, true);
WshShell.Run ("cmd /C \"del Y:\\bin\\winnt\\htk_camview.exe\"", 7, true);
