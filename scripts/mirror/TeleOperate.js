var WshShell = WScript.CreateObject("WScript.Shell");

// launch camview
WshShell.Run ("on -d -n atlas \"camview.exe --name /TCSaccades/i:img --flip --l --x 1 --y 1 --w 900 --h 900 --net Net1 --p 100\"", 7, true);

// launch mirror collector
WshShell.Run ("on -d -n atlas \"mirrorCollector\"", 7, true);

// launch TeleCtrl
WshShell.Run ("on -d -n atlas \"TeleCtrl\"", 7, true);
