var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n cariddi \"camview.exe --name /viewtracker/i:img --x 761 --y 0 --net Net1 --p 100\"", 7, true);

