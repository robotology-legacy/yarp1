var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("porter /left/o:img !/handlocalization/i:img", 7, true); 
WshShell.Run ("porter /handlocalization/o:img !/view006/i:img", 7, true);
WshShell.Run ("porter /handlocalization/o:pixel !/vect/i", 7, true);
// WshShell.Run ("porter /view006/o:point !/handlocalization/i:point", 7, true);
WshShell.Run ("porter /handcontrol/o:status !/handlocalization/i:hand", 7, true);
WshShell.Run ("porter /repeater/o !/handlocalization/behavior/i", 7, true);

WshShell.Run ("porter /left/o:img !/handtracker/i:img", 7, true);

WshShell.Run ("slay -n calliope handlocalizatio", 7, true);
WshShell.Run ("slay -n leto handcolortracke", 7, true);

WshShell.Run ("slay -n leto camview.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe", 7, true);
WshShell.Run ("slay -n calliope camview.exe", 7, true);
// WshShell.Run ("slay -n leto vectviewer.exe", 7, true);
