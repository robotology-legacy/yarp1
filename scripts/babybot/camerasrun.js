var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n hades \"framegrab.exe -b 0 +left -l -n Net1 -o 4\"", 7, true);
WshShell.Run ("on -d -n hades \"framegrab.exe -b 1 +right -l -n Net1 -o -4\"", 7, true);

WshShell.Run ("on -d -n euterpe \"camview.exe -name view001/i:img -l -x 1 -y 1 -net Net1 -p 100\"", 7, true);
WshShell.Run ("on -d -n euterpe \"camview.exe -name view002/i:img -l -x 250 -y 1 -net Net1 -p 100\"", 7, true);
WshShell.Run ("on -d -n euterpe \"camview.exe -name view003/i:img -l -f -x 1 -y 270 -net Net1\"", 7, true);
WshShell.Run ("on -d -n euterpe \"camview.exe -name view004/i:img -l -f -x 250 -y 270 -net Net1 -p 100\"", 7, true);