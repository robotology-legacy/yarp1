var WshShell = WScript.CreateObject("WScript.Shell");

// disconnect
WshShell.Run("porter /nnet1/o !/handtracker/nnet1/i", 7, true);
WshShell.Run ("porter /nnet2/o !/handtracker/nnet2/i", 7, true);

WshShell.Run ("porter /handtracker/nnet1/o !/nnet1/i", 7, true);
WshShell.Run ("porter /handtracker/nnet2/o !/nnet2/i", 7, true);

// connect
WshShell.Run ("porter /nnet1/o /handtracker/nnet1/i", 7, true);
WshShell.Run ("porter /nnet2/o /handtracker/nnet2/i", 7, true);

WshShell.Run ("porter /handtracker/nnet1/o /nnet1/i", 7, true);
WshShell.Run ("porter /handtracker/nnet2/o /nnet2/i", 7, true);








