var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter.exe /headcontrol/inertial/o !/vor/inertial/i", 7, true);
WshShell.Run ("porter.exe /tracker/o:vect !/headsmooth/i", 7, true);
WshShell.Run ("porter.exe /vergence/o:disparity !/headvergence/i", 7, true);

WshShell.Run ("porter.exe /vor/o !/headsink/vor/i", 7, true);
WshShell.Run ("porter.exe /headsmooth/o !/headsink/track/i", 7, true);
WshShell.Run ("porter.exe /headvergence/o !/headsink/vergence/i", 7, true);

WshShell.Run ("porter.exe /headsink/o !/headcontrol/i", 7, true);
WshShell.Run ("porter.exe /repeater/o !/headsink/behavior/i", 7, true);
WshShell.Run ("porter.exe /headcontrol/position/o !/headsink/position/i", 7, true);

WshShell.Run ("porter.exe /repeater/o !/attention/behavior/i", 7, true);

WshShell.Run ("porter.exe /tracker/o:vect !/attention/target/i", 7, true);
WshShell.Run ("porter.exe /handtracker/position/o !/attention/hand/i", 7, true);
WshShell.Run ("porter.exe /handtracker/prediction/o !/attention/handprediction/i", 7, true);

WshShell.Run ("porter.exe /attention/O !/headsmooth/i", 7, true);