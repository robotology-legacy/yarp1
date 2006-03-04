// create instance of JScript
var WshShell = WScript.CreateObject("WScript.Shell");

// try and disconnect first
WshShell.Run ("BodyMapDisconnect.js", 7, true);

// connect BodyMap to mirrorCollector
WshShell.Run ("yarp-connect /mirrorCollector/o:str  /BodyMap/i:str", 7, true);
WshShell.Run ("yarp-connect /mirrorCollector/o:img0 /BodyMap/i:img0", 7, true);
WshShell.Run ("yarp-connect /mirrorCollector/o:img1 /BodyMap/i:img1", 7, true);
WshShell.Run ("yarp-connect /mirrorCollector/o:int  /BodyMap/i:int", 7, true);
WshShell.Run ("yarp-connect /BodyMap/o:int          /mirrorCollector/i:int", 7, true);

// connect BodyMap to mirrorCollector
WshShell.Run ("yarp-connect /learner/o:vec  /BodyMapL/i:vec", 7, true);
WshShell.Run ("yarp-connect /learner/o:int  /BodyMapL/i:int", 7, true);
WshShell.Run ("yarp-connect /BodyMapL/o:vec /learner/i:vec", 7, true);
WshShell.Run ("yarp-connect /BodyMapL/o:int /learner/i:int", 7, true);
