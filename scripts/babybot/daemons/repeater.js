var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -i -n eolo \"repeater.exe --name /repeater\"", 7, true);