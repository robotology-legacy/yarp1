var WshShell = WScript.CreateObject("WScript.Shell");

var dest1 = "c:/winnt/system32/repl/import/scripts/logon.bat";		// NT
var dest2 = "c:/windows/system32/repl/import/scripts/logon.bat";	//xp, 2k

var s1 = "z:/logon1.bat";
var s2 = "z:/logon2.bat";

WScript.Echo("Executing script..." );

var filesystem = new ActiveXObject("Scripting.FileSystemObject");

try
{
 	filesystem.CopyFile(s1,dest1);
}
catch(e)
{
	// file1 failed trying second file
	WScript.Echo("Unable to copy " + s1 + " to " + dest1);	
	try	
	{
	 	filesystem.CopyFile(s2,dest2);
	}
	catch(e)
	{
		WScript.Echo("Unable to copy " + s2 + " to " + dest2);	
	}
}



