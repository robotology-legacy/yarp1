var WshShell = WScript.CreateObject("WScript.Shell");
//WshShell.Run ("on -i -d -n leto \"trackercloned.exe -name trackercloned -neti Net1 -neto Net1\"", 7, true);

//WshShell.Run ("on -d -n leto \"camview.exe -name pfview/i:img -l -x 500 -y 1 -net Net1\"", 7, true);
//WshShell.Run ("on -d -n euterpe \"vectviewer.exe -name viewer/tracker/i -size 2 -net default -length 250 -height 70 -x 0\"", 7, true);

//WshShell.Run ("on -d -n cariddi \"camview.exe -name pfview3/i:img -l -x 500 -y 1 -net Net1\"", 7, true);
//WshShell.Run ("on -i -d -n cariddi \"trackercloned.exe -name trackercloned -neti Net1 -neto Net1\"", 7, true);


WshShell.Run ("on  -d -n leto \"trackercloned.exe --name /trackercloned --neti Net1 --neto Net1\"", 7, true);
//WshShell.Run ("on  -d -n cariddi \"trackercloned.exe -name trackercloned -neti Net1 -neto Net1\"", 7, true);
