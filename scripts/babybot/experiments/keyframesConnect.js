var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter.exe /left/o:img !/keyframes/i:left", 7, true);
WshShell.Run ("porter.exe /keyframes/o:img !/keyframesview/i:img", 7, true);
WshShell.Run ("porter.exe /armcontrol/o:status !/keyframes/arm/i", 7, true);
WshShell.Run ("porter.exe /keyframes/features/o:img !/keyframesview2/i:img", 7, true);
WshShell.Run ("porter.exe /keyframes/prediction/o !/keyframesview3/i:img", 7, true);
WshShell.Run ("porter.exe /repeater/o !/keyframes/repeater/i", 7, true);

WshShell.Run ("porter.exe /left/o:img /keyframes/i:left", 7, true);
WshShell.Run ("porter.exe /keyframes/o:img /keyframesview/i:img", 7, true);

WshShell.Run ("porter.exe /keyframes/features/o:img /keyframesview2/i:img", 7, true);

WshShell.Run ("porter.exe /keyframes/prediction/o /keyframesview3/i:img", 7, true);
WshShell.Run ("porter.exe /repeater/o /keyframes/repeater/i", 7, true);

WshShell.Run ("porter.exe /armcontrol/o:status /keyframes/arm/i", 7, true);