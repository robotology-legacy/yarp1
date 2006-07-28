var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("explorationDisconnect.js", 7, true);

WshShell.Run ("slay -n rhea kfexploration.exe", 7, true);
