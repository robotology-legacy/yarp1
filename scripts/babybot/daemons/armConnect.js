var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("armDisconnect.js", 7, true);

WshShell.Run ("yarp-connect /armcontrol/behavior/o /repeater/i", 7, true);
WshShell.Run ("yarp-connect /repeater/o /armcontrol/behavior/i", 7, true);


