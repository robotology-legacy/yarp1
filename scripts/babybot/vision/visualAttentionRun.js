var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n scilla visualattention.exe --name /left", 7, true);

WshShell.Run ("on -d -n hades \"camview.exe --l --name /visualattention/view1/left/i:img --x 1 --y 300 --net Net1 --p 100\"", 7,true);
WshShell.Run ("on -d -n hades \"camview.exe --l --name /visualattention/view2/left/i:img --x 250 --y 300 --net Net1 --p 100\"", 7,true);
WshShell.Run ("on -d -n hades \"camview.exe --l --name /visualattention/view3/left/i:img --x 500 --y 300 --net Net1 --p 100\"", 7,true);
