var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -i -d -n circe \"vergence.exe --name /vergence --neti Net1 --neto Net0\"", 7, true);
WshShell.Run ("cmd /C \"copy Y:\\bin\\winnt\\camview.exe Y:\\bin\\winnt\\vrg_camview.exe\"", 7, true);
WshShell.Run ("on -d -n circe \"vrg_camview.exe --name /viewvergence/i:img --l --x 500 --y 10 --w 400 --h 180 --net Net1 --p 100 --w 260 --h 140 \"", 7, true);
