var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("porter /left/o:img !/view001/i:img", 7, true);
WshShell.Run ("porter /right/o:img !/view002/i:img", 7, true);

WshShell.Run ("porter /left/o:img !/view003/i:img", 7, true);
WshShell.Run ("porter /right/o:img !/view004/i:img", 7, true);

//WshShell.Run ("slay -n leto framegrab.exe", 7, true);
WshShell.Run ("slay -n euterpe camview.exe", 7, true);