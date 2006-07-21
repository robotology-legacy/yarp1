var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect /visualattention/left/o:img !/visualattention/view1/left/i:img", 7, true);
WshShell.Run ("yarp-connect /visualattention/left/o:img2 !/visualattention/view2/left/i:img", 7, true);
WshShell.Run ("yarp-connect /visualattention/left/o:img3 !/visualattention/view3/left/i:img", 7, true);

WshShell.Run ("yarp-connect /left/o:img !/visualattention/left/i:img", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o !/visualattention/left/i:vec", 7, true);
WshShell.Run ("yarp-connect /checkfixation/out !/visualattention/left/i:vec2", 7, true);
