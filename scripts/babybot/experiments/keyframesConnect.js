var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect.exe /left/o:img !/keyframes/i:left", 7, true);
WshShell.Run ("yarp-connect.exe /keyframes/o:img !/keyframesview/i:img", 7, true);
WshShell.Run ("yarp-connect.exe /armcontrol/o:status !/keyframes/arm/i", 7, true);
WshShell.Run ("yarp-connect.exe /keyframes/features/o:img !/keyframesview2/i:img", 7, true);
WshShell.Run ("yarp-connect.exe /keyframes/prediction/o !/keyframesview3/i:img", 7, true);
WshShell.Run ("yarp-connect.exe /repeater/o !/keyframes/repeater/i", 7, true);
WshShell.Run ("yarp-connect.exe /keyframes/tracker/o !/attention/target/i", 7, true);
WshShell.Run ("yarp-connect.exe /keyframes/tracker/o !/smoothpursuit/i", 7, true);

WshShell.Run ("yarp-connect.exe /left/o:img /keyframes/i:left", 7, true);
WshShell.Run ("yarp-connect.exe /keyframes/o:img /keyframesview/i:img", 7, true);

WshShell.Run ("yarp-connect.exe /keyframes/features/o:img /keyframesview2/i:img", 7, true);

WshShell.Run ("yarp-connect.exe /keyframes/prediction/o /keyframesview3/i:img", 7, true);
WshShell.Run ("yarp-connect.exe /repeater/o /keyframes/repeater/i", 7, true);

WshShell.Run ("yarp-connect.exe /armcontrol/o:status /keyframes/arm/i", 7, true);

WshShell.Run ("yarp-connect.exe /keyframes/tracker/o /smoothpursuit/i", 7, true);







