var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("graspReflexDisconnect.js", 7, true);

WshShell.Run ("slay -n rhea grasprflx.exe", 7, true);
