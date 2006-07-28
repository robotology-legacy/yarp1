var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("slay -n rhea handlocalization.exe", 7, true);
WshShell.Run ("slay -n rhea hloc_camview.exe", 7, true);
WshShell.Run ("cmd /C \"del Y:\\bin\\winnt\\hloc_camview.exe\"", 7, true);
// WshShell.Run ("slay -n calliope vectviewer.exe", 7, true);
