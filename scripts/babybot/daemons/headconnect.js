var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("headDisconnect.js", 7, true);

WshShell.Run ("yarp-connect /headcontrol/behavior/o /repeater/i", 7, true);
WshShell.Run ("yarp-connect /repeater/o /headcontrol/behavior/i", 7, true);


