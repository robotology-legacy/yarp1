var WshShell = WScript.CreateObject("WScript.Shell");

// hand tracker-> 
// -position plot position
// -prediction plot prediction
// threshold was 3000000
WshShell.Run ("on -d -i -n oceanus \"handcolortracker.exe -position -prediction -threshold 0\"", 1, true);
