var WshShell = WScript.CreateObject("WScript.Shell");

//WshShell.Run ("porter /pf/o:img !/pfview1/i:img", 7, true);

WshShell.Run ("slay -n leto grabber.exe", 7, true);
WshShell.Run ("slay -n leto camview.exe", 7, true);
