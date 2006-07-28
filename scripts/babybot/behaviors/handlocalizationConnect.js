var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("handLocalizationDisconnect.js", 7, true);

WshShell.Run ("yarp-connect /left/o:img /handlocalization/i:img", 7, true);
WshShell.Run ("yarp-connect /handlocalization/o:img /view006/i:img", 7, true);

WshShell.Run ("yarp-connect /handlocalization/segmentation/o:img /view007/i:img", 7, true);
// WshShell.Run ("yarp-connect /handlocalization/o:pixel /vect/i", 7, true);
WshShell.Run ("yarp-connect /armcontrol/o:status /handlocalization/i:motor", 7, true);
WshShell.Run ("yarp-connect /armcontrol/o:status /handtracker/i:arm", 7, true);
WshShell.Run ("yarp-connect /repeater/o /handlocalization/behavior/i", 7, true);

WshShell.Run ("yarp-connect /handlocalization/segmentation/o:img /handtracker/segmentation/i:img", 7, true);

WshShell.Run ("yarp-connect /left/o:img /handtracker/i:img", 7, true);
WshShell.Run ("yarp-connect /handtracker/segmentation/o:img /view008/i:img", 7, true);
WshShell.Run ("yarp-connect /handtracker/backprojection/o:img /view009/i:img", 7, true);
WshShell.Run ("yarp-connect /handlocalization/segmentation/o:armdata /handtracker/segmentation/i:armdata", 7, true);
