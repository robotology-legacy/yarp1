var WshShell = WScript.CreateObject("WScript.Shell");

// WshShell.Run ("on -d -n leto \"seqgrabber.exe -neti default -name motiongrabber -length 20\"", 7, true);
// WshShell.Run ("on -d -n leto \"seqgrabber.exe -neti default -name imagesgrabber -length 150 -dir C:/temp/seq6\"", 7, true);
WshShell.Run ("on -d -n leto \"seqgrabber.exe -neti Net1 -name imagesgrabber -length 20 -dir C:/temp/cogvis1\"", 7, true);