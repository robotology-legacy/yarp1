var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("slay -n oceanus framegrab.exe", 1, true);
WshShell.Run ("slay -n euterpe camview.exe", 1, true);