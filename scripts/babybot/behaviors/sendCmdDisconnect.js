var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect.exe /motorcmd/o !/repeater/i", 7, true);

