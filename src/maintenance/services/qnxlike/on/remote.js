var objArgs = WScript.Arguments;

var scriptName = objArgs(0);
var remoteMachine = objArgs(1);

WScript.Echo("Executing " + scriptName + " on " + remoteMachine);

var script;

// script = "CScript " + "scriptName";
script = scriptName;

var oController = new ActiveXObject("WSHController");
var oProcess = oController.CreateScript(script, remoteMachine);
oProcess.Execute();
while(oProcess.Status != 2) WScript.Sleep(100);
WScript.Echo("Done");