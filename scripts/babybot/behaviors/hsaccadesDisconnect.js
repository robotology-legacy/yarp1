var WshShell = WScript.CreateObject("WScript.Shell");

// disconnect visual attention module
// WshShell.Run ("porter /headsaccades/attention/o !/visualattention/left/i:bot", 7, true);
// WshShell.Run ("porter /visualattention/left/o:bot !/headsaccades/target/i", 7, true);


WshShell.Run ("porter /headsaccades/cmd/o !/headsink/saccades/i", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/headsaccades/position/i", 7, true);
WshShell.Run ("porter /bottlesender/o !/headsaccades/behavior/i", 7, true);

WshShell.Run ("porter /vergence/o:disparity !/checkfixation/disparity/i", 7, true);
WshShell.Run ("porter /headsaccades/fixation/o !/checkfixation/target/i", 7, true);
WshShell.Run ("porter /checkfixation/out !/headsaccades/fixation/i", 7, true);

WshShell.Run ("porter /left/o:img !/saccadesview/i:img", 7, true);
WshShell.Run ("porter /point/out !/headsaccades/target/i", 7, true);