var WshShell = WScript.CreateObject("WScript.Shell");

// WshShell.Run ("on -d -n circe \"seqgrabber.exe --neti default --name /motiongrabber --length 20\"", 7, true);
// WshShell.Run ("on -d -n circe \"seqgrabber.exe --neti default --name /imagesgrabber --length 150 --dir C:/temp/seq6\"", 7, true);
WshShell.Run ("on -d -n circe \"seqgrabber.exe --neti Net1 --name /imagesgrabber --length 120 -dir C:/temp/tmp --stereo\"", 7, true);