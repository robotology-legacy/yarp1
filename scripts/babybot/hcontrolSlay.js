var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("hControlDisconnect.js", 7, true);

WshShell.Run ("slay -n thaumas vor.exe", 7, true);
WshShell.Run ("slay -n thaumas headsink.exe", 7, true);
WshShell.Run ("slay -n thaumas headsmooth.exe", 7, true);
WshShell.Run ("slay -n thaumas headvergence.ex", 7, true);
WshShell.Run ("slay -n thaumas attention.exe", 7, true);

