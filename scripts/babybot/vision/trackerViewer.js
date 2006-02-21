var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n cariddi \"camview.exe --name /viewtracker/i:img --x 501 --y 270 --net Net1 --p 100\"", 7, true);

