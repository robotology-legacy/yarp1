var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /left/o:img !/egomap/image/i", 7, true);
WshShell.Run ("yarp-connect /egomap/image/o !/egomapview/i:img", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o !/egomap/head/i", 7, true);

