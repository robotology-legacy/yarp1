var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("slay -n leto visualattention.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe", 7, true);
