var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /repeater/o !/collect/behavior/i", 7, true);
WshShell.Run ("porter /armcontrol/o:status !/collect/arm/i", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/collect/head/i", 7, true);
WshShell.Run ("porter /vergence/o:disparity !/collect/vergence/i", 7, true);
WshShell.Run ("porter /handtracker/position/o !/collect/target/i", 7, true);