var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /repeater/o /armrandom/behavior/i", 7, true);
WshShell.Run ("yarp-connect /armrandom/behavior/o /repeater/i", 7, true);