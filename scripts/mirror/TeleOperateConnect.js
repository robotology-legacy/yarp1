var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("TeleOperateDisconnect.js", 7, true);

WshShell.Run ("yarp-connect /mirrorCollector/o:str /TCCollector/i:str", 7, true);
WshShell.Run ("yarp-connect /mirrorCollector/o:int /TCCollector/i:int", 7, true);
WshShell.Run ("yarp-connect /TCCollector/o:int     /mirrorCollector/i:int", 7, true);
WshShell.Run ("yarp-connect /TCRepeater/o:str      /repeater/i", 7, true);

WshShell.Run ("yarp-connect /left/o:img            /TCImg/i:img", 7, true);
WshShell.Run ("yarp-connect /TCImg/o:img           /TCSaccades/i:img", 7, true);

WshShell.Run ("yarp-connect /TCHeadSaccades/o:bot /headsaccades/target/i", 7, true);

WshShell.Run ("yarp-connect /left/o:img            /service/i:img", 7, true);
