var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("porter /left/o:img !/visualattention/left/i:img", 7, true);
WshShell.Run ("porter /headcontrol/position/o !/visualattention/left/i:vec", 7, true);

WshShell.Run ("porter /visualattention/left/o:img !/visualattention/view/left/i:img", 7, true);


WshShell.Run ("porter /headcontrol/position/o /visualattention/left/i:vec", 7, true);
WshShell.Run ("porter /left/o:img /visualattention/left/i:img", 7, true);

WshShell.Run ("porter /visualattention/left/o:img /visualattention/view/left/i:img", 7, true);