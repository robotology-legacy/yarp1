var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n scilla \"learner.exe --dom 3 --cod 6 --ex 3000\"", 7, true);

