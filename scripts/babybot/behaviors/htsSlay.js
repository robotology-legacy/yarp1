var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("htsDisconnect.js", 7, true);
WshShell.Run ("slay -n proteus handtrackerswit", 7, true);
