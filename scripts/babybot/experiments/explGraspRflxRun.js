var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n rhea grasprflx.exe --norelease --cfg grasprflxKF.ini", 7, true);
