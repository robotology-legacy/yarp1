var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n circe \"seqgrabber.exe --neti Net1 --name /imagesgrabber --dir c:/temp/calib --stop --stereo\"", 7, true);
