var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n calliope keyframes.exe", 7, true);
WshShell.Run ("on -d -n calliope \"camview.exe --name /keyframesview/i:img --x 1 --y 300 --net default --p 100\"", 7, true);
WshShell.Run ("on -d -n calliope \"camview.exe --name /keyframesview2/i:img --x 250 --y 300 --net default --p 100\"", 7, true);
WshShell.Run ("on -d -n calliope \"camview.exe --ame /keyframesview3/i:img --x 500 --y 300 --net default --p 500\"", 7, true);