var WshShell = WScript.CreateObject("WScript.Shell");

/// disconnect first

//WshShell.Run ("porter /left/o:img !/trackercloned/i:img", 7, true);
//WshShell.Run ("porter /trackercloned/o:img !/pfview3/i:img", 7, true);
//WshShell.Run ("porter /trackercloned/o:img !/pfview4/i:img", 7, true);

WshShell.Run ("yarp-connect /trackercloned/o:img !/pfview5/i:img", 7, true);
WshShell.Run ("yarp-connect /cart/o:img !/trackercloned/i:img", 7, true);
WshShell.Run ("yarp-connect /trackercloned/o:img !/pfview5/i:img", 7, true);
WshShell.Run ("yarp-connect /kfexploration/behavior/o !/trackercloned/i:bot", 7, true);
WshShell.Run ("yarp-connect /bstop/o !/trackercloned/i:bot", 7, true);
WshShell.Run ("yarp-connect /bstart/o !/trackercloned/i:bot", 7, true);
WshShell.Run ("yarp-connect /trackercloned/o:bot !/headsaccades/target/i", 7, true);
WshShell.Run ("yarp-connect /checkfixation/bottle/out !/trackercloned/i:bot", 7, true);

WshShell.Run ("yarp-connect /kfexploration/behavior/o !/trackercloned/i:bot", 7, true);

