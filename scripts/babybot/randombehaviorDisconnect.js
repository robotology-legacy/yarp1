var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /repeater/o !/armrandom/behavior/i", 7, true);
WshShell.Run ("porter /armrandom/behavior/o !/repeater/i", 7, true);