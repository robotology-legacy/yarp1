var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("hsaccadesDisconnect.js", 7, true);

WshShell.Run ("slay -n thaumas headsaccades.ex", 7, true);
WshShell.Run ("slay -n thaumas checkfixation.e", 7, true);

WshShell.Run ("slay -n circe hsc_camview.exe", 7, true);
WshShell.Run ("cmd /C \"del Y:\\bin\\winnt\\hsc_camview.exe\"", 7, true);
