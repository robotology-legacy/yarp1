var WshShell = WScript.CreateObject("WScript.Shell");

// reaching
WshShell.Run ("porter /armcontrol/o:status /reaching/arm/i", 7, true);
WshShell.Run ("porter /headcontrol/position/o /reaching/head/i", 7, true);
WshShell.Run ("porter /repeater/o /reaching/behavior/i", 7, true);
WshShell.Run ("porter /reaching/behavior/o /repeater/i", 7, true);

// arm trigger
WshShell.Run ("porter /trigger/behavior/o /repeater/i", 7, true);
WshShell.Run ("porter /repeater/o /trigger/behavior/i", 7, true);

//WshShell.Run ("porter /handtracker/position/o /trigger/hand/i", 7, true);
//WshShell.Run ("porter /tracker/o:vect /trigger/target/i", 7, true);

WshShell.Run ("porter /attention/o /trigger/target/i", 7, true);
WshShell.Run ("porter /vergence/o:disparity /trigger/vergence/i", 7, true);

WshShell.Run ("porter /trigger/egomap/o /egomap/cmd/i", 7, true);