var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("porter /right/o:img !/vergence/i:right", 7, true);
WshShell.Run ("porter /left/o:img !/vergence/i:right", 7, true);
WshShell.Run ("porter /left/o:img !/vergence/i:left", 7, true);

WshShell.Run ("porter /headcontrol/position/o !/vergence/head/i", 7, true);
WshShell.Run ("porter.exe /vergence/o:disparity !/headvergence/i", 7, true);

WshShell.Run ("porter /vergence/o:histo !/viewvergence/i:img", 7, true);
//WshShell.Run ("porter /vergence/o:histo2 !/viewvergence2/i:img", 7, true);
//WshShell.Run ("porter /vergence/o:img !/viewvergence3/i:img", 7, true);
//WshShell.Run ("porter /vergence/o:img2 !/viewvergence4/i:img", 7, true);
//WshShell.Run ("porter /vergence/o:img3 !/viewvergence5/i:img", 7, true);
//WshShell.Run ("porter /vergence/o:img4 !/viewvergence6/i:img", 7, true);
