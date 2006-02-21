var WshShell = WScript.CreateObject("WScript.Shell");

// disconnect first
WshShell.Run("vergenceDisconnect.js", 7, true);

/// slay
WshShell.Run ("slay -n circe vergence.exe", 7, true);
WshShell.Run ("slay -n circe camview.exe ", 7, true);
