var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n circe \"camview.exe --name /paxis/view1/i:img --x 250 --y 1 --net Net1 --p 100\"", 7,true);
