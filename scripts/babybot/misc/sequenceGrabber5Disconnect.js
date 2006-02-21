var WshShell = WScript.CreateObject("WScript.Shell");
// WshShell.Run ("on -d -n eolo \"yarp-connect /handlocalization/o:img  !/motiongrabber/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /left/o:img !/imggrb/i:left\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /right/o:img !/imggrb/i:right\"", 7, true);

WshShell.Run ("on -d -n eolo \"yarp-connect /visualattention/left/o:img !/imggrb/i:3\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /visualattention/left/o:img2 !/imggrb/i:4\"", 7, true);
//WshShell.Run ("on -d -n eolo \"yarp-connect /visualattention/left/o:img3 !/imggrb/i:5\"", 7, true);
WshShell.Run ("on -d -n eolo \"yarp-connect /paxis/o:img !/imggrb/i:5\"", 7, true);