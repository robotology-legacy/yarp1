var WshShell = WScript.CreateObject("WScript.Shell");

// WshShell.Run ("on -d -n leto \"seqgrabber.exe -neti default -name motiongrabber -length 10\"", 7, true);
WshShell.Run ("on -d -n leto \"seqgrabber.exe -neti default -name imagesgrabber -length 10\"", 7, true);
