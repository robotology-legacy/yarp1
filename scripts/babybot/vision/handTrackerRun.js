var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -i -n hades \"handcolortracker.exe --position --threshold 1\"", 7, true);
WshShell.Run ("on -d -n hades \"camview.exe --name /view008/i:img --p 100 --x 1 --y 1\"", 7, true);
