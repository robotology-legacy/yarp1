var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /handcontrol/behavior/o !/repeater/i", 7, true);
WshShell.Run ("yarp-connect /repeater/o !/handcontrol/behavior/i", 7, true);