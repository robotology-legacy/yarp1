var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n eolo \"yarp-connect /armcontrol/o:status /datagrabber/arm/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /headcontrol/position/o /datagrabber/head/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /handcontrol/o:status /datagrabber/hand/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /touch/o /datagrabber/touch/i\"", 7, true);
// WshShell.Run ("on -d -n eolo \"yarp-connect /attention/o /datagrabber/retinal/i\"", 7, true);

WshShell.Run ("on -d -n eolo \"yarp-connect /right/o:img /datagrabber/right/i:img\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /left/o:img /datagrabber/left/i:img\"", 7, true);