var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n polifemo \"remotelearn.exe -name rnnet -conf reaching.ini -epoch 900000 -trainset reaching.dat -outnet reaching.out\"", 1, true);