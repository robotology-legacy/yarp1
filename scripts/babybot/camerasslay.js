var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("slay -n oceanus framegrab.exe", 7, true);
WshShell.Run ("slay -n euterpe camview.exe", 7, true);