var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("porter.exe /kfexploration/behavior/o !/keyframes/behavior/i", 7, true);

WshShell.Run ("porter.exe /repeater/o !/kfexploration/i", 7, true);
WshShell.Run ("porter.exe /kfexploration/o !/repeater/i", 7, true);

WshShell.Run ("porter.exe /repeater/o /kfexploration/i", 7, true);
WshShell.Run ("porter.exe /kfexploration/o /repeater/i", 7, true);

WshShell.Run ("porter.exe /kfexploration/behavior/o /keyframes/behavior/i", 7, true);

