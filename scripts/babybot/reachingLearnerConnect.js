var WshShell = WScript.CreateObject("WScript.Shell");

// disconnect
WshShell.Run("porter /rnnet/o !/reaching/nnet/i", 7, true);
WshShell.Run ("porter /reaching/nnet/o !rnnet/i", 7, true);

// connect
WshShell.Run ("porter /rnnet/o /reaching/nnet/i", 7, true);
WshShell.Run ("porter /reaching/nnet/o /rnnet/i", 7, true);


