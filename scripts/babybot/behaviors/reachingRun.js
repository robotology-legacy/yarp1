var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n proteus reaching.exe", 7, true);

//WshShell.Run ("on -d -n rhea \"matlab71 /r handkinematics\"", 7, true);
//WshShell.Run ("on -d -n rhea armtrigger.exe", 7, true);

WshShell.Run ("on -d -n pento \"matlab /r cd('y:/src/experiments/babybot/handkinematics'),handkinematics\"", 7, true);
WshShell.Run ("on -d -n pento armtrigger.exe", 7, true);
