var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect /headcontrol/position/o !/paxis/i:vec", 7, true);
WshShell.Run ("yarp-connect /left/o:img !/paxis/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img !/paxis/i:img2", 7, true);
WshShell.Run ("yarp-connect /paxis/o:img !/paxis/view1/i:img", 7, true);
WshShell.Run ("yarp-connect /paxis/o:bot !/reaching/orientation/i:bot", 7, true);
WshShell.Run ("yarp-connect /visualattention/left/o:img2 !/paxis/i:img3", 7, true);
