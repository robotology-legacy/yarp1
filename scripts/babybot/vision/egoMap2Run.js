var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n leto egomap.exe", 7, true);
WshShell.Run ("on -d -n \"leto camview --name /egomapview/i:img\"", 7, true);