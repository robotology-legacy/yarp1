var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("hsaccadesDisconnect.js", 7, true);

WshShell.Run ("slay -n thaumas headsaccades.ex", 7, true);
WshShell.Run ("slay -n thaumas checkfixation.e", 7, true);
WshShell.Run ("slay -n circe camview.exe", 7, true);
