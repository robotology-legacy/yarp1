var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -n proteus armcontrol.exe", 1, false);
WshShell.Run ("behaviorrnd.exe", 1, false);
WshShell.Run ("sendcmd.exe", 1, false);
WshShell.Run ("repeater.exe -name repeater/", 1, false);