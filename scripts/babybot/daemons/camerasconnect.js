var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect /left/o:img !/view001/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img !/view002/i:img", 7, true);
WshShell.Run ("yarp-connect /left/o:img !/view003/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img !/view004/i:img", 7, true);

WshShell.Run ("yarp-connect /left/o:img /view001/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img /view002/i:img", 7, true);
WshShell.Run ("yarp-connect /left/o:img /view003/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img /view004/i:img", 7, true);

// test stereomatch
//WshShell.Run ("yarp-connect /left/o:img !/sm/i:left", 7, true);
//WshShell.Run ("yarp-connect /right/o:img !/sm/i:right", 7, true);
//WshShell.Run ("yarp-connect /sm/o:img !/viewsm/i:img", 7, true);
//WshShell.Run ("yarp-connect /left/o:img /sm/i:left", 7, true);
//WshShell.Run ("yarp-connect /right/o:img /sm/i:right", 7, true);
//WshShell.Run ("yarp-connect /sm/o:img /viewsm/i:img", 7, true);
