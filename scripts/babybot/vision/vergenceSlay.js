var WshShell = WScript.CreateObject("WScript.Shell");

// disconnect first
WshShell.Run("vergenceDisconnect.js", 7, true);

/// slay
WshShell.Run ("slay -n calliope vergence.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe ", 7, true);
