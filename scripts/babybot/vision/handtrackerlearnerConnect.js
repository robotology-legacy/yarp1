var WshShell = WScript.CreateObject("WScript.Shell");

// disconnect
WshShell.Run("yarp-connect /nnet1/o !/handtracker/nnet1/i", 7, true);
WshShell.Run ("yarp-connect /nnet2/o !/handtracker/nnet2/i", 7, true);

WshShell.Run ("yarp-connect /handtracker/nnet1/o !/nnet1/i", 7, true);
WshShell.Run ("yarp-connect /handtracker/nnet2/o !/nnet2/i", 7, true);

// connect
WshShell.Run ("yarp-connect /nnet1/o /handtracker/nnet1/i", 7, true);
WshShell.Run ("yarp-connect /nnet2/o /handtracker/nnet2/i", 7, true);

WshShell.Run ("yarp-connect /handtracker/nnet1/o /nnet1/i", 7, true);
WshShell.Run ("yarp-connect /handtracker/nnet2/o /nnet2/i", 7, true);








