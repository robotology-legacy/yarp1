var WshShell = WScript.CreateObject("WScript.Shell");

// launch two grabbers, two CAMView's and connect the bunch

WshShell.Run ("yarp-connect /left/o:img !/TeleCtrlLView/i:img", 7, false);
WshShell.Run ("yarp-connect /right/o:img !/TeleCtrlRView/i:img", 7, false);

WshShell.Run ("slay -n polifemo grabber.exe", 7, true);
WshShell.Run ("slay -n atlas camview.exe", 7, true);
WshShell.Run ("slay -n atlas mirrorCollector.exe", 7, true);
WshShell.Run ("slay -n atlas TeleCtrl.exe", 7, true);
