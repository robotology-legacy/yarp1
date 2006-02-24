var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("sendCmdDisconnect.js", 7, true);

WshShell.Run ("slay -n eolo sendcmd.exe", 7, true);
