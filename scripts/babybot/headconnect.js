var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /headcontrol/behavior/o /repeater/i", 7, true);
WshShell.Run ("porter /repeater/o /headcontrol/behavior/i", 7, true);


