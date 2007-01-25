var WshShell = WScript.CreateObject("WScript.Shell");

// launch mirror collector
WshShell.Run ("on -d -n atlas \"mirrorCollector\"", 7, true);

// launch camview
WshShell.Run ("on -d -n atlas \"camview.exe --name /TCSaccades/i:img --flip --x 1 --y 1 --w 700 --h 700 --net Net1 --p 100\"", 7, true);

// launch TeleCtrl
WshShell.Run ("on -d -n atlas \"TeleCtrl\"", 7, true);

// service camview
//WshShell.Run ("on -d -n atlas \"camview.exe --name /service/i:img --l --flip --x 1 --y 1 --w 700 --h 700 --net Net1 --p 100\"", 7, true);
