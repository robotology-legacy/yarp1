var WshShell = WScript.CreateObject("WScript.Shell");

/// run the vergence code.
WshShell.Run ("on -i -d -n oceanus \"vergence.exe -name vergence -neti Net1 -neto Net1\"", 7, true);

/// run the viewer.
WshShell.Run ("on -d -n oceanus \"camview.exe -name viewvergence/i:img -l -x 300 -y 400 -net Net1 -p 100 -w 300 -h 170\"", 7, true);
