var WshShell = WScript.CreateObject("WScript.Shell");

var baseName = "MEM_Acceptor_*.*";

var filesystem = new ActiveXObject("Scripting.FileSystemObject");
var fullName1 = "%TEMP%\\" + baseName;
var fullName2 = "%windir%\\" + baseName;

completeFiles1 = WshShell.ExpandEnvironmentStrings(fullName1);
completeFiles2 = WshShell.ExpandEnvironmentStrings(fullName2);

filesystem.DeleteFile(completeFiles1, true);
filesystem.DeleteFile(completeFiles2, true);