var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("testjacobianDisconnect.js", 7, true);
WshShell.Run ("slay -n circe testjacobian.exe", 7, true);
