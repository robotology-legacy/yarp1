var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first
WshShell.Run ("stereoMatchDisconnect.js", 7, true);

/// slay viewer
WshShell.Run ("slay -n calliope camview.exe", 7, true);
/// run the program
WshShell.Run ("slay -n calliope teststereomatch", 7, true);