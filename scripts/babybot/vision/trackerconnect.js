var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /left/o:img /tracker/i:img", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o /tracker/i:headposition", 7, true);
WshShell.Run ("yarp-connect /tracker/o:vect /attention/target/i", 7, true);

// disconnect from frameg.
WshShell.Run ("yarp-connect /left/o:img !/view003/i:img", 7, true);

// reattach to display
WshShell.Run ("yarp-connect /tracker/o:img /view003/i:img", 7, true);
