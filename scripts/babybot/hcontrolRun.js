var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -i -d -n thaumas vor.exe", 7, true);
WshShell.Run ("on -d -n thaumas headsink.exe", 7, true);
WshShell.Run ("on -i -d -n thaumas headsmooth.exe", 7, true);
WshShell.Run ("on -i -d -n thaumas headvergence.exe", 7, true);