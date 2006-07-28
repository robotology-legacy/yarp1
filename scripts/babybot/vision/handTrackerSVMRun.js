var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -i -n hades \"handcolortrackersvm.exe --position --threshold 1\"", 7, true);
WshShell.Run ("cmd /C \"copy Y:\\bin\\winnt\\camview.exe Y:\\bin\\winnt\\htk_camview.exe\"", 7, true);
WshShell.Run ("on -d -n hades \"htk_camview.exe --name /view008/i:img --p 100 --x 1 --y 1\"", 7, true);
WshShell.Run ("on -d -n hades \"htk_camview.exe --name /view009/i:img --p 100 --x 300 --y 1\"", 7, true);
