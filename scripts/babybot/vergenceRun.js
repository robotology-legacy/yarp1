var WshShell = WScript.CreateObject("WScript.Shell");

/// run the vergence code.
WshShell.Run ("on -d -n calliope \"vergence.exe -name vergence -neti Net1 -neto Net0\"", 7, true);

/// run the viewer.
WshShell.Run ("on -d -n calliope \"camview.exe -name viewvergence/i:img -l -x 300 -y 200 -net Net1 -p 100 -w 250 -h 260 \"", 7, true);
