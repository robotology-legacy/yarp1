var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp-connect.exe /kfexploration/behavior/o !/keyframes/behavior/i", 7, true);
WshShell.Run ("yarp-connect.exe /repeater/o !/kfexploration/i", 7, true);
WshShell.Run ("yarp-connect.exe /kfexploration/o !/repeater/i", 7, true);

WshShell.Run ("slay -n pento kfexploration.exe", 7, true);