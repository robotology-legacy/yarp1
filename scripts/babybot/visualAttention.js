var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("on -d -n scilla visualattention.exe -name left", 7, true);

WshShell.Run ("on -d -n calliope \"camview.exe -l -name visualattention/view/left/i:img -x 250 -y 1 -net Net1 -p 100\"", 7, true);