var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -i -d -n leto \"tracker.exe --name /tracker --neti Net1 --neto default\"", 7, true);

//WshShell.Run ("on -d -n leto \"camview.exe --name view0010/i:img --l --x 500 --y 1 --net Net1\"", 7, true);
//WshShell.Run ("on -d -n leto \"vectviewer.exe --name /viewer/tracker/i --size 2 --net default --length 250 --height 70 --x 0\"", 7, true);