// Teleoperate.js
//
// this script activates teleoperation between the MIRROR and the BABYBOT setups.
// the script presumes that the ARM is waiting for commands.
// then it
// (.) launches two Cartesian grabbers on polifemo
// (.) launches two camviews on atlas
// (.) connects the grabbers to the camviewes
// (.) launches mirrorCollector on atlas
// (.) launches TeleCtrl on atlas
// All being well, the MIRROR setup will now drive the Babybot.
//

// create instance of JScript
var WshShell = WScript.CreateObject("WScript.Shell");

// launch two grabbers, two CAMView's and connect the bunch
WshShell.Run ("on -d -n polifemo \"grabber.exe --b 0 --name /left --net Net1 --o 4\"", 7, true);
WshShell.Run ("on -d -n polifemo \"grabber.exe --b 1 --name /right --net Net1 --o -4\"", 7, true);
WshShell.Run ("on -d -n atlas \"camview.exe --name /TeleCtrlLView/i:img --x 1 --y 1 --net Net1 --p 100\"", 7, true);
WshShell.Run ("on -d -n atlas \"camview.exe --name /TeleCtrlRView/i:img --x 1 --y 270 --net Net1 --p 100\"", 7, true);
WshShell.Run ("yarp-connect /left/o:img !/TeleCtrlLView/i:img", 7, false);
WshShell.Run ("yarp-connect /right/o:img !/TeleCtrlRView/i:img", 7, false);
WshShell.Run ("yarp-connect /left/o:img /TeleCtrlLView/i:img", 7, false);
WshShell.Run ("yarp-connect /right/o:img /TeleCtrlRView/i:img", 7, false);

// launch mirror collector
WshShell.Run ("on -d -n atlas \"mirrorCollector\"", 7, true);

// launch TeleCtrl. TeleCtrl will try and connect to mirrorCollector (master) and to /repeater/i (slave)
WshShell.Run ("on -d -n atlas \"TeleCtrl\"", 7, true);
