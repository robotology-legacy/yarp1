var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /armcontrol/behavior/o /repeater/i", 7, false);
WshShell.Run ("porter /repeater/o /armcontrol/behavior/i", 7, false);


