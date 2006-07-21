var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n thaumas headsaccades.exe --strict_reaching", 7, true);
WshShell.Run ("on -d -n thaumas checkfixation.exe", 7, true);

WshShell.Run ("on -d -n circe \"camview.exe --name /saccadesview/i:img --l --x 1 --y 1 --w 300 --h 300 --net Net1 --out /point/out --p 100\"", 7, true);