var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run("vergenceDisconnect.js", 7, true);

WshShell.Run ("slay -n circe vergence.exe", 7, true);
WshShell.Run ("slay -n circe vrg_camview.exe ", 7, true);
WshShell.Run ("cmd /C \"del Y:\\bin\\winnt\\vrg_camview.exe\"", 7, true);
