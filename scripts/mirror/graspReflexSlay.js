var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("graspReflexDisconnect.js", 7, false);

WshShell.Run ("slay -n eolo grasprflx.exe", 7, true);
