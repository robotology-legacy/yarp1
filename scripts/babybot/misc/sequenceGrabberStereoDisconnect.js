var WshShell = WScript.CreateObject("WScript.Shell");
// WshShell.Run ("on -d -n eolo \"porter /handlocalization/o:img  !/motiongrabber/i\"", 7, true);
WshShell.Run ("on -d -n eolo \"porter /left/o:img !/imagesgrabber/i:left\"", 7, true);
WshShell.Run ("on -d -n eolo \"porter /right/o:img !/imagesgrabber/i:right\"", 7, true);

// WshShell.Run ("on -d -n eolo \"porter /handtracker/segmentation/o:img !/imagesgrabber/i\"", 7, true);