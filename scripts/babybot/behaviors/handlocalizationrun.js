var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n rhea handlocalization.exe -t 10 -a 0.2", 7, true);

WshShell.Run ("cmd /C \"copy Y:\\bin\\winnt\\camview.exe Y:\\bin\\winnt\\hloc_camview.exe\"", 7, true);
WshShell.Run ("on -d -n rhea \"hloc_camview.exe --name /view006/i:img --l --x 1 --y 1\"", 7, true);
WshShell.Run ("on -d -n rhea \"hloc_camview.exe --name /view007/i:img --l --x 250 --y 1\"", 7, true);

// WshShell.Run ("on -d -n calliope \"vectview.exe --name vect/i --size 4 --scale 1.0 --x 500\"", 7, true);
