var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect.exe /headcontrol/inertial/o !/vor/inertial/i", 7, true);
WshShell.Run ("yarp-connect.exe /tracker/o:vect !/headsmooth/i", 7, true);
WshShell.Run ("yarp-connect.exe /vergence/o:disparity !/headvergence/i", 7, true);

WshShell.Run ("yarp-connect.exe /vor/o !/headsink/vor/i", 7, true);
WshShell.Run ("yarp-connect.exe /headsmooth/o !/headsink/track/i", 7, true);
WshShell.Run ("yarp-connect.exe /headvergence/o !/headsink/vergence/i", 7, true);

WshShell.Run ("yarp-connect.exe /headsink/o !/headcontrol/i", 7, true);
WshShell.Run ("yarp-connect.exe /repeater/o !/headsink/behavior/i", 7, true);
WshShell.Run ("yarp-connect.exe /headcontrol/position/o !/headsink/position/i", 7, true);

WshShell.Run ("yarp-connect.exe /smoothpursuit/o !/headsink/smoothpursuit/i", 7, true);

WshShell.Run ("yarp-connect.exe /repeater/o !/attention/behavior/i", 7, true);

WshShell.Run ("yarp-connect.exe /tracker/o:vect !/attention/target/i", 7, true);
WshShell.Run ("yarp-connect.exe /handtracker/position/o !/attention/hand/i", 7, true);
WshShell.Run ("yarp-connect.exe /handtracker/prediction/o !/attention/handprediction/i", 7, true);

WshShell.Run ("yarp-connect.exe /attention/o !/headsmooth/i", 7, true);