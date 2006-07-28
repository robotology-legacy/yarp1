var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("handTrackerLearnerSVMDisconnect.js", 7, true);

//WshShell.Run ("yarp-connect /learner/o:vec  /BodyMapL/i:vec", 7, true);
//WshShell.Run ("yarp-connect /learner/o:int  /BodyMapL/i:int", 7, true);
WshShell.Run ("yarp-connect /handtracker/nnet1/o /learner/i:vec", 7, true);
//WshShell.Run ("yarp-connect /BodyMapL/o:int /learner/i:int", 7, true);







