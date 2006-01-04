var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("camview --name /lview/i:img", 7, false);
WshShell.Run ("camview --name /rview/i:img", 7, false);
WshShell.Run ("Release\mirrorCollector", 7, true);
