var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect /testjacobian/o:img !/jacview/i:img", 7, true);
WshShell.Run ("yarp-connect /jacview/o:point !/testjacobian/target/i", 7, true);
WshShell.Run ("yarp-connect /left/o:img !/testjacobian/i:img", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o !/testjacobian/head/i", 7, true);
WshShell.Run ("yarp-connect /armcontrol/o:status !/testjacobian/arm/i", 7, true);
WshShell.Run ("yarp-connect /testjacobian/arm/o !/repeater/i", 7, true);
