var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("mirrorCaptureDisconnect.js", 7, true);

WshShell.Run ("yarp-connect /mirrorCollector/o:img0 /mirrorGrasp/i:img0", 7, true);
WshShell.Run ("yarp-connect /mirrorCollector/o:img1 /mirrorGrasp/i:img1", 7, true);
WshShell.Run ("yarp-connect /mirrorCollector/o:str /mirrorGrasp/i:str", 7, true);
WshShell.Run ("yarp-connect /mirrorGrasp/o:int /mirrorCollector/i:int", 7, true);
WshShell.Run ("yarp-connect /mirrorCollector/o:int /mirrorGrasp/i:int", 7, true);
