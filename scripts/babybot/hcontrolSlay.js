var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("porter.exe /headsink/o !/headcontrol/i", 7, true);

WshShell.Run ("porter.exe /headcontrol/inertial/o !/vor/inertial/i", 7, true);
WshShell.Run ("porter.exe /tracker/o:vect !/headsmooth/i", 7, true);
WshShell.Run ("porter.exe /vor/o !/headsink/vor/i", 7, true);
WshShell.Run ("porter.exe /headsmooth/o !/headsink/track/i", 7, true);
wshShell.Run ("porter.exe /repeater/o !/headsink/behavior/i", 7, true);
WshShell.Run ("porter.exe /headcontrol/position/o !/headsink/position/i", 7, true);

WshShell.Run ("slay -n thaumas vor.exe", 7, true);
WshShell.Run ("slay -n thaumas headsink.exe", 7, true);
WshShell.Run ("slay -n thaumas headsmooth.exe", 7, true);
