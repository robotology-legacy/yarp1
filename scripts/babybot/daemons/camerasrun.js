var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n polifemo \"grabber-logpolar.exe --b 0 --name /left --l --net Net1 --o 4\"", 7, true);
WshShell.Run ("on -d -n polifemo \"grabber-logpolar.exe --b 1 --name /right --l --net Net1 --o 12\"", 7, true);

WshShell.Run ("cmd /C \"copy Y:\\bin\\winnt\\camview.exe Y:\\bin\\winnt\\cam_camview.exe\"", 7, true);

WshShell.Run ("on -d -n cariddi \"cam_camview.exe --name /view001/i:img --l --x 1 --y 1 --w 380 --h 380 --net Net1 --p 100\"", 7, true);
WshShell.Run ("on -d -n cariddi \"cam_camview.exe --name /view002/i:img --l --x 380 --y 1 --w 380 --h 380 --net Net1 --p 100\"", 7, true);
WshShell.Run ("on -d -n cariddi \"cam_camview.exe --name /view003/i:img --l --f --x 1 --y 380 --w 380 --h 380 --net Net1 --p 100\"", 7, true);
WshShell.Run ("on -d -n cariddi \"cam_camview.exe --name /view004/i:img --l --f --x 380 --y 380 --w 380 --h 380 --net Net1 --p 100\"", 7, true);
