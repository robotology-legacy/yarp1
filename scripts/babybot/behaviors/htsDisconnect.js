var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /repeater/o !/hts/i", 7, true);
WshShell.Run ("porter /hts/o !/repeater/i", 7, true);