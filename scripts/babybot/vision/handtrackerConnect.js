var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /armcontrol/o:status /handtracker/arm/i", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o /handtracker/head/i", 7, true);

WshShell.Run ("yarp-connect /handlocalization/segmentation/o:img /view007/i:img", 7, true);
WshShell.Run ("yarp-connect /handlocalization/segmentation/o:img /handtracker/segmentation/i:img", 7, true);

WshShell.Run ("yarp-connect /left/o:img /handtracker/i:img", 7, true);
WshShell.Run ("yarp-connect /handtracker/segmentation/o:img /view008/i:img", 7, true);
//WshShell.Run ("yarp-connect /handtracker/backprojection/o:img /view009/i:img", 7, true);
WshShell.Run ("yarp-connect /repeater/o /handtracker/prediction/i", 7, true);

WshShell.Run ("yarp-connect /handtracker/prediction/o /attention/handprediction/i", 7, true);
WshShell.Run ("yarp-connect /handtracker/position/o /attention/hand/i", 7, true);











