var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -i -d -n cariddi \"tracker.exe --name /tracker --neti Net1 --neto default\"", 7, true);
WshShell.Run ("cmd /C \"copy Y:\\bin\\winnt\\camview.exe Y:\\bin\\winnt\\trk_camview.exe\"", 7, true);
WshShell.Run ("on -d -n cariddi \"trk_camview.exe --name /viewtracker/i:img --x 761 --y 0 --net Net1 --p 100\"", 7, true);
