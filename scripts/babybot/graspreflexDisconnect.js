var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter.exe /touch/o !/touch/i", 7, false);
WshShell.Run ("porter.exe /graspreflex/o !/repeater/i", 7, false);
WshShell.Run ("porter.exe /repeater/o !/graspreflex/i", 7, false);