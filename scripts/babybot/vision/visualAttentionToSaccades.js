var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect /headsaccades/attention/o !/visualattention/left/i:bot", 7, true);
WshShell.Run ("yarp-connect /visualattention/left/o:bot !/headsaccades/target/i", 7, true);

// connect visual attention to headsaccades and the opposite
WshShell.Run ("yarp-connect /headsaccades/attention/o /visualattention/left/i:bot", 7, true);
WshShell.Run ("yarp-connect /visualattention/left/o:bot /headsaccades/target/i", 7, true);

