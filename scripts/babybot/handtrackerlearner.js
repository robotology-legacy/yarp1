var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n pento \"remotelearn.exe -name nnet1 -conf handfk1.ini -epoch 900000 -trainset handfk1.dat -outnet handfk1.out\"", 1, true);
WshShell.Run ("on -d -n pento \"remotelearn.exe -name nnet2 -conf handfk2.ini -epoch 900000 -trainset handfk2.dat -outnet handfk2.out\"", 1, true);
