var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("porter.exe /left/o:img !/visualattention/left/i:img", 7, true);

WshShell.Run ("porter.exe /visualattention/left/o:img !/visualattention/view/left/i:img", 7, true);

WshShell.Run ("porter.exe /left/o:img /visualattention/left/i:img", 7, true);

WshShell.Run ("porter.exe /visualattention/left/o:img /visualattention/view/left/i:img", 7, true);