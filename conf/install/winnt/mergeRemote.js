// this script can be used to merge reg files on a remote machine
// it uses the "on" batch command.
// Before you use this script you have to be sure that:
// - "on" is installed and in your path (local machine only)
// - %YARP_ROOT% is define on the remote machine
// - remote scripts are enabled on the remote machine (see install.txt on the same folder)
//
// syntax: mergeRemote machine regfile
// regFile has to be located in "%YARP_ROOT%\conf\install\winnt\" 
// NOTE: you can use regedit /s if you want to disable messages. In this case remember
// error messages are not shown...
// 
// July 2003 -- by nat

var objArgs = WScript.Arguments;

var remoteMachine = objArgs(0);
var regFile = objArgs(1);

var WshShell = WScript.CreateObject("WScript.Shell");

// var objEnv = WshShell.Environment("System");
var path = "%YARP_ROOT%\\\\conf\\\\\install\\\\winnt\\\\";

completeRegFile = WshShell.ExpandEnvironmentStrings(path)+regFile;

WshShell.Run ("on -d -n "+remoteMachine+" \"regedit " + completeRegFile + "\"", 1, true);