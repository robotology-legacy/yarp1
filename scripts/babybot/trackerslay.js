var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first
WshShell.Run ("trackerdisconnect.js", 7, true);

/// slay
WshShell.Run ("slay -n euterpe tracker.exe", 7, true);
WshShell.Run ("slay -n euterpe vectviewer.exe ", 7, true);
