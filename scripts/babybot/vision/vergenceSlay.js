var WshShell = WScript.CreateObject("WScript.Shell");

/// slay
WshShell.Run ("slay -n oceanus vergence.exe", 7, true);
WshShell.Run ("slay -n oceanus camview.exe ", 7, true);
