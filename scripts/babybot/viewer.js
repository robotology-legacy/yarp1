var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run ("vectviewer.exe -name vect/i -size 5 -length 200 -height 80 -x 0", 1, true);