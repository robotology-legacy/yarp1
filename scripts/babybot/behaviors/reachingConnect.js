var WshShell = WScript.CreateObject("WScript.Shell");

// reaching
WshShell.Run ("porter /armcontrol/o:status /reaching/arm/i", 7, true);
WshShell.Run ("porter /headcontrol/position/o /reaching/head/i", 7, true);
WshShell.Run ("porter /repeater/o /reaching/behavior/i", 7, true);
WshShell.Run ("porter /reaching/behavior/o /repeater/i", 7, true);

// arm trigger
WshShell.Run ("porter /armtrigger/behavior/o /repeater/i", 7, true);
WshShell.Run ("porter /repeater/o /armtrigger/behavior/i", 7, true);
WshShell.Run ("porter /checkfixation/bottle/out /armtrigger/behavior/i", 7, true);

