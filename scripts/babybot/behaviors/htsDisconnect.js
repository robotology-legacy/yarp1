var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /repeater/o !/hts/i", 7, true);
WshShell.Run ("yarp-connect /hts/o !/repeater/i", 7, true);