var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /visualattention/left/o:img !/visualattention/view1/left/i:img", 7, true);
WshShell.Run ("porter /visualattention/left/o:img !/visualattention/view2/left/i:img", 7, true);

WshShell.Run ("porter /visualattention/left/o:img /visualattention/view1/left/i:img", 7, true);
WshShell.Run ("porter /visualattention/left/o:img2 /visualattention/view2/left/i:img", 7, true);