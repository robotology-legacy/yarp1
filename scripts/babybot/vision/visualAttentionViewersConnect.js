var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("visualAttentionViewersDisconnect.js", 7, true);

WshShell.Run ("yarp-connect /visualattention/left/o:img /visualattention/view1/left/i:img", 7, true);
WshShell.Run ("yarp-connect /visualattention/left/o:img2 /visualattention/view2/left/i:img", 7, true);
WshShell.Run ("yarp-connect /visualattention/left/o:img3 /visualattention/view3/left/i:img", 7, true);