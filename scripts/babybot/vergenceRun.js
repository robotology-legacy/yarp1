var WshShell = WScript.CreateObject("WScript.Shell");

/// run the vergence code.
WshShell.Run ("on -d -n calliope \"vergence.exe -name vergence -neti Net1 -neto default\"", 7, true);

/// run the viewer.
WshShell.Run ("on -d -n calliope \"camview.exe -name viewvergence/i:img -l -x 500 -y 1 -net Net1 -p 100\"", 7, true);
