var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("TeleOperationDisconnect.js", 7, true);

WshShell.Run ("slay -n atlas camview.exe", 7, true);
WshShell.Run ("slay -n atlas mirrorCollector.exe", 7, true);
WshShell.Run ("slay -n atlas TeleCtrl.exe", 7, true);
