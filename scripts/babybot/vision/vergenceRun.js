var WshShell = WScript.CreateObject("WScript.Shell");

/// run the vergence code.
WshShell.Run ("on -d -n oceanus \"vergence.exe -name vergence -neti Net1 -neto Net0\"", 7, true);

/// run the viewer.
WshShell.Run ("on -d -n oceanus \"camview.exe -name viewvergence/i:img -l -x 10 -y 10 -net Net1 -p 100 -w 250 -h 260 \"", 7, true);
WshShell.Run ("on -d -n oceanus \"camview.exe -name viewvergence2/i:img -l -x 260 -y 10 -net Net1 -p 100 -w 250 -h 260 \"", 7, true);
WshShell.Run ("on -d -n oceanus \"camview.exe -name viewvergence3/i:img -l -x 10 -y 300 -net Net1 -p 100 -w 250 -h 260 \"", 7, true);
WshShell.Run ("on -d -n oceanus \"camview.exe -name viewvergence4/i:img -l -x 260 -y 300 -net Net1 -p 100 -w 250 -h 260 \"", 7, true);
//WshShell.Run ("on -d -n oceanus \"camview.exe -name viewvergence5/i:img -l -x 520 -y 300 -net Net1 -p 100 -w 250 -h 260 \"", 7, true);
//WshShell.Run ("on -d -n oceanus \"camview.exe -name viewvergence6/i:img -l -x 520 -y 10 -net Net1 -p 100 -w 250 -h 260 \"", 7, true);
