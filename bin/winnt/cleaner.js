var WshShell = WScript.CreateObject("WScript.Shell");

var baseName = "MEM_Acceptor_*.*";

var filesystem = new ActiveXObject("Scripting.FileSystemObject");
var fullName1 = "%Temp%\\" + baseName;
var fullName2 = "%windir%\\" + baseName;
var fullName3 = "C:\\Temp\\" + baseName;

var completeFiles1 = WshShell.ExpandEnvironmentStrings(fullName1);
var completeFiles2 = WshShell.ExpandEnvironmentStrings(fullName2);
var completeFiles3 = WshShell.ExpandEnvironmentStrings(fullName3);

// WScript.Echo(completeFiles1);
// WScript.Echo(completeFiles2);
// WScript.Echo(completeFiles3);

try
{
	filesystem.DeleteFile(completeFiles1,false);
}
catch(e){}

try
{
	filesystem.DeleteFile(completeFiles2,false);
}
catch(e){}

try
{
	filesystem.DeleteFile(completeFiles3,false);
}
catch(e){}