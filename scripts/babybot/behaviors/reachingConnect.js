var WshShell = WScript.CreateObject("WScript.Shell");

// reaching
// WshShell.Run ("yarp-connect /armcontrol/o:status /reaching/arm/i", 7, true);
// WshShell.Run ("yarp-connect /headcontrol/position/o /reaching/head/i", 7, true);
WshShell.Run ("yarp-connect /repeater/o /reaching/behavior/i", 7, true);
WshShell.Run ("yarp-connect /reaching/behavior/o /repeater/i", 7, true);

// arm trigger
WshShell.Run ("yarp-connect /armtrigger/behavior/o /repeater/i", 7, true);
WshShell.Run ("yarp-connect /repeater/o /armtrigger/behavior/i", 7, true);
WshShell.Run ("yarp-connect /checkfixation/bottle/out /armtrigger/behavior/i", 7, true);

