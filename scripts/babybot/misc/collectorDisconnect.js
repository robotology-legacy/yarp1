var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /repeater/o !/collect/behavior/i", 7, true);
WshShell.Run ("yarp-connect /armcontrol/o:status !/collect/arm/i", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o !/collect/head/i", 7, true);
WshShell.Run ("yarp-connect /vergence/o:disparity !/collect/vergence/i", 7, true);
WshShell.Run ("yarp-connect /handtracker/position/o !/collect/target/i", 7, true);