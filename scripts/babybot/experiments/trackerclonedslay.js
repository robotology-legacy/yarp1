var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first
WshShell.Run ("trackercloneddisconnect.js", 7, true);

/// slay

//WshShell.Run ("slay -n cariddi trackercloned.exe", 7, true);
WshShell.Run ("slay -n leto trackercloned.exe", 7, true);

//WshShell.Run ("slay -n cariddi camview.exe", 7, true);

