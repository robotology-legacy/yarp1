var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n pento grasprflx.exe -norelease -cfg grasprflxKF.ini", 7, true);