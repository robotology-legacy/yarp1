var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n calliope handlocalization.exe -t 10 -a 0.2", 1, true);

WshShell.Run ("on -d -n leto handcolortracker.exe", 1, true);

WshShell.Run ("on -d -n calliope \"camview.exe -name view006/i:img -l -x 1 -y 1\"", 1, true);
WshShell.Run ("on -d -n calliope \"camview.exe -name view007/i:img -l -x 250 -y 1\"", 1, true);

WshShell.Run ("on -d -n leto \"camview.exe -name view008/i:img -p 400 -x 1 -y 1\"", 1, true);
WshShell.Run ("on -d -n leto \"camview.exe -name view009/i:img -l -p 400 -x 250 -y 1\"", 1, true);
// WshShell.Run ("on -d -n calliope \"vectviewer.exe -name vect/i -size 4 -scale 1.0 -x 500\"", 1, true);


