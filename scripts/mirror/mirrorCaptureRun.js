var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n atlas \"mirrorCollector\"", 7, true);

WshShell.Run ("on -d -n atlas \"graspcapture\"", 7, true);
