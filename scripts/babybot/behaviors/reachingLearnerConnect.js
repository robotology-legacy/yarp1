var WshShell = WScript.CreateObject("WScript.Shell");

// disconnect
WshShell.Run("yarp-connect /rnnet/o !/reaching/nnet/i", 7, true);
WshShell.Run ("yarp-connect /reaching/nnet/o !/rnnet/i", 7, true);

// connect
WshShell.Run ("yarp-connect /rnnet/o /reaching/nnet/i", 7, true);
WshShell.Run ("yarp-connect /reaching/nnet/o /rnnet/i", 7, true);


