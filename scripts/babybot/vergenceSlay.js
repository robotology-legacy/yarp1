var WshShell = WScript.CreateObject("WScript.Shell");

/// slay
WshShell.Run ("slay -n calliope vergence.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe ", 7, true);
