var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect.exe /touch/o !/touch/i", 7, true);
WshShell.Run ("yarp-connect.exe /graspreflex/o !/repeater/i", 7, true);
WshShell.Run ("yarp-connect.exe /repeater/o !/graspreflex/i", 7, true);