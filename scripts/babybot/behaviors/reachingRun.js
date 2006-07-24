var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n rhea \"matlab71 /r handkinematics\"", 7, true);
WshShell.Run ("on -d -n proteus reaching.exe", 7, true);
WshShell.Run ("on -d -n rhea armtrigger.exe", 7, true);
