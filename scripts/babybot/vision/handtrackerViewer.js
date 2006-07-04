var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n hades \"camview.exe --name /view008/i:img --p 100 --x 1 --y 1\"", 7, true);
//WshShell.Run ("on -d -n oceanus \"camview.exe --name /view009/i:img --l --p 100 --x 250 --y 1\"", 7, true);
