var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n eolo \"porter /right/o:img !/datagrabber/right/i:img\"", 7, true);
WshShell.Run ("on -d -n eolo \"porter /left/o:img !/datagrabber/left/i:img\"", 7, true);

WshShell.Run ("on -d -n eolo \"porter /armcontrol/o:status !/datagrabber/arm/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"porter /headcontrol/position/o !/datagrabber/head/i\"", 7, true);

WshShell.Run ("on -d -n eolo \"porter /handcontrol/o:status !/datagrabber/hand/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"porter /touch/o !/datagrabber/touch/i\"", 7, true);

// WshShell.Run ("on -d -n eolo \"porter /attention/o !/datagrabber/retinal/i\"", 7, true);