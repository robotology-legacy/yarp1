var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n circe \"camview.exe --name /jacview/i:img --x 1 --y 1 --out /jacview/o:point --net Net0 --p 100\"", 7, true);
