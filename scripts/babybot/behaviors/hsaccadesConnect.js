var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("hsaccadesDisconnect.js", 7, true);

WshShell.Run ("yarp-connect /headsaccades/cmd/o /headsink/saccades/i", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o /headsaccades/position/i", 7, true);
WshShell.Run ("yarp-connect /bottlesender/o /headsaccades/behavior/i", 7, true);
WshShell.Run ("yarp-connect /headsaccades/behavior/o /repeater/i", 7, true);

WshShell.Run ("yarp-connect /vergence/o:disparity /checkfixation/disparity/i", 7, true);
WshShell.Run ("yarp-connect /headsaccades/fixation/o /checkfixation/target/i", 7, true);
WshShell.Run ("yarp-connect /checkfixation/out /headsaccades/fixation/i", 7, true);

WshShell.Run ("yarp-connect /left/o:img /saccadesview/i:img", 7, true);
WshShell.Run ("yarp-connect /point/out /headsaccades/target/i", 7, true);


// connect visual attention to headsaccades and the opposite
// WshShell.Run ("yarp-connect /headsaccades/attention/o /visualattention/left/i:bot", 7, true);
// WshShell.Run ("yarp-connect /visualattention/left/o:bot /headsaccades/target/i", 7, true);

