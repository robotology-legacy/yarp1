var WshShell = WScript.CreateObject("WScript.Shell");

// hand tracker-> 
// -position plot position
// -prediction plot prediction
// threshold was 3000000
//WshShell.Run ("on -d -i -n oceanus \"handcolortracker.exe --position --prediction --threshold 1\"", 7, true);
WshShell.Run ("on -d -i -n oceanus \"handcolortracker.exe --position --threshold 1\"", 7, true);
