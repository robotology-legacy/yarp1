var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter /left/o:img /teststereomatch/i:left", 7, true);
WshShell.Run ("porter /right/o:img /teststereomatch/i:right", 7, true);
WshShell.Run ("porter /teststereomatch/o:img /view/stereomatch", 7, true);

