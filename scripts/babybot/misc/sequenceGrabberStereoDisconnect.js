var WshShell = WScript.CreateObject("WScript.Shell");
// WshShell.Run ("on -d -n eolo \"yarp-connect /handlocalization/o:img  !/motiongrabber/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /left/o:img !/imagesgrabber/i:left\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /right/o:img !/imagesgrabber/i:right\"", 7, true);

// WshShell.Run ("on -d -n eolo \"yarp-connect /handtracker/segmentation/o:img !/imagesgrabber/i\"", 7, true);