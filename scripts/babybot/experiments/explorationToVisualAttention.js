var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect.exe /kfexploration/behavior/o !/visualattention/left/i:bot", 7, true);
//WshShell.Run ("yarp-connect.exe /kfexploration/behavior/o !/shapehist/i:bot", 7, true);

WshShell.Run ("yarp-connect.exe /kfexploration/behavior/o /visualattention/left/i:bot", 7, true);
//WshShell.Run ("yarp-connect.exe /kfexploration/behavior/o /shapehist/i:bot", 7, true);
