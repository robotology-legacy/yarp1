var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /left/o:img /teststereomatch/i:left", 7, true);
WshShell.Run ("yarp-connect /right/o:img /teststereomatch/i:right", 7, true);
WshShell.Run ("yarp-connect /teststereomatch/o:img /view/stereomatch", 7, true);

