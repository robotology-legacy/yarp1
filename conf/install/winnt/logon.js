var shareName = "\\\\eolo\\yarp";
var drive = "Y:";

WScript.Echo("Executing logon script...");

var net = WScript.CreateObject("WSCript.Network");

WScript.Echo();
WScript.Echo("Hello from " + net.ComputerName);
WScript.Echo("Logged on as " + net.UserName + " at " + net.UserDomain);
WScript.Echo();

WScript.Echo("Mapping: " + shareName + " on " + drive + " as " + net.UserName + " pass: not shown");

MapDrive(drive, shareName, net.UserName);

WScript.Echo("done !");

function MapDrive(drive, share, user)
{
	wshNetwork = WScript.CreateObject("WSCript.Network");
	// wshNetwork.MapNetworkDrive(drive, share, false, user, pass);
	wshNetwork.MapNetworkDrive(drive, share, false, user);
}