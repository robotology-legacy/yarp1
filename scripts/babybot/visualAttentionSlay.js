var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("slay -n scilla visualattention.exe", 7, true);
WshShell.Run ("slay -n leto camview.exe", 7, true);
