var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n polifemo handlocalization.exe -t 10 -a 0.2", 1, true);

WshShell.Run ("on -d -n polifemo \"camview.exe -name view006/i:img -l -x 1 -y 1\"", 1, true);
WshShell.Run ("on -d -n polifemo \"camview.exe -name view007/i:img -l -x 250 -y 1\"", 1, true);

// WshShell.Run ("on -d -n calliope \"vectviewer.exe -name vect/i -size 4 -scale 1.0 -x 500\"", 1, true);


