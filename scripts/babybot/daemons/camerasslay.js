var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect /left/o:img !/view001/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img !/view002/i:img", 7, true);

WshShell.Run ("yarp-connect /left/o:img !/view003/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img !/view004/i:img", 7, true);

WshShell.Run ("slay -n hades grabber-logpola", 7, true);
WshShell.Run ("slay -n euterpe camview.exe", 7, true);