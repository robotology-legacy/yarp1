var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("yarp-connect /trackercloned/o:img /pfview5/i:img", 7, true);

