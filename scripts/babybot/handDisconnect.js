var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /handcontrol/behavior/o !/repeater/i", 7, true);
WshShell.Run ("porter /repeater/o !/handcontrol/behavior/i", 7, true);