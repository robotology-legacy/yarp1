var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("collectorDisconnect.js", 7, true);
WshShell.Run ("yarp-connect /armcontrol/o:status /datacollector/arm/i", 7, true);
WshShell.Run ("yarp-connect /repeater/o /datacollector/synchro", 7, true);
WshShell.Run ("yarp-connect /headcontrol/position/o /datacollector/head/i", 7, true);
WshShell.Run ("yarp-connect /handcontrol/o:status /datacollector/hand/i", 7, true);
WshShell.Run ("yarp-connect /touch/o /datacollector/touch/i", 7, true);
WshShell.Run ("yarp-connect /left/o:img /datacollector/left/i:img", 7, true);
WshShell.Run ("yarp-connect /right/o:img /datacollector/right/i:img", 7, true);
