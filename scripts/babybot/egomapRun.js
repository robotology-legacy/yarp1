var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n leto \"egomap.exe -name egomap -neti Net1 -neto default\"", 7, true);
WshShell.Run ("on -d -n leto \"camview.exe -name viewegomap/i:img -l -x 500 -y 1 -net Net1\"", 7, true);
