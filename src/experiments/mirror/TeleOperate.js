//
// this script presumes that BabyBot is already set up and waiting for commands
//

// create instance of JScript
var WshShell = WScript.CreateObject("WScript.Shell");

// launch two CAMView's and connect them to he cameras
WshShell.Run ("camview --name /TeleCtrlLView/i:img --net Net1", 7, false);
WshShell.Run ("camview --name /TeleCtrlRView/i:img --net Net1", 7, false);
WshShell.Run ("yarp-connect /left/o:img !/TeleCtrlLView/i:img", 7, false);
WshShell.Run ("yarp-connect /right/o:img !/TeleCtrlRView/i:img", 7, false);
WshShell.Run ("yarp-connect /left/o:img /TeleCtrlLView/i:img", 7, false);
WshShell.Run ("yarp-connect /right/o:img /TeleCtrlRView/i:img", 7, false);

// launch mirror collector
//WshShell.Run ("C:\yarp\src\experiments\mirror\mirrorCollector\Release\mirrorCollector.exe", 7, true);

// launch TeleCtrl. TeleCtrl will try and connect to mirrorCollector (master)
// AND to /repeater/i (slave)
//WshShell.Run ("C:\yarp\src\experiments\mirror\TeleCtrl\Release\TeleCtrl.exe", 7, true);
