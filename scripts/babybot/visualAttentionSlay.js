var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("slay -n scilla visualattention.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe", 7, true);
