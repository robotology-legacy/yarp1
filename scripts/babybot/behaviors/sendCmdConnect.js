var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("sendCmdDisconnect.js", 7, true);
WshShell.Run ("yarp-connect.exe /motorcmd/o /repeater/i", 7, true);

