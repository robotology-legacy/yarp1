//
// this script presumes that BabyBot is already set up and waiting for commands
//

// create instance of JScript
var WshShell = WScript.CreateObject("WScript.Shell");

// launch two CAMView's and connect them to he cameras
WshShell.Run ("camview --name /TeleCtrlLView/i:img", 7, false);
WshShell.Run ("camview --name /TeleCtrlRview/i:img", 7, false);
WshShell.Run ("yarp-connect /left/o:img !/TeleCtrlLView/i:img", 7, false);
WshShell.Run ("yarp-connect /right/o:img !/TeleCtrlRview/i:img", 7, false);
WshShell.Run ("yarp-connect /left/o:img /TeleCtrlLView/i:img", 7, false);
WshShell.Run ("yarp-connect /right/o:img /TeleCtrlRview/i:img", 7, false);

// launch mirror collector
WshShell.Run ("mirrorCollector\Release\mirrorCollector", 7, true);

// launch TeleCtrl. TeleCtrl will try and connect to mirrorCollector (master)
// AND to /repeater/i (slave)
WshShell.Run ("TeleCtrl\Release\TeleCtrl", 7, true);
