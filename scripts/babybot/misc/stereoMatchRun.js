var WshShell = WScript.CreateObject("WScript.Shell");

// viewer
WshShell.Run ("on -d -n calliope \"camview.exe --name /view/stereomatch --x 1 --y 1 --p 100\"", 7, true);

// run the program
WshShell.Run ("on -i -d -n calliope teststereomatch.exe", 7, true);
