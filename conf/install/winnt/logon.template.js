// by nat Jan 2003
// handle exception, Dec 2003

var shareName = "\\\\eolo\\yarp"; //better use the ip number
var drive = "Y:";

WScript.Echo("Executing logon script...");

var net = WScript.CreateObject("WSCript.Network");

WScript.Echo();
WScript.Echo("Hello from " + net.ComputerName);
WScript.Echo("Logged on as " + net.UserName + " at " + net.UserDomain);
WScript.Echo();

WScript.Echo("Mapping: " + shareName + " on " + drive + " as " + net.UserName + " pass: not shown");

while (MapDrive(drive, shareName, net.UserName) == 0)
{
	WScript.Echo("Unable to map "+ shareName + " on " + drive +" trying again");
	WScript.Sleep(500);
}

WScript.Echo("done !");

function MapDrive(drive, share, user, pass)
{
	try
	{
		wshNetwork = WScript.CreateObject("WSCript.Network");
		wshNetwork.MapNetworkDrive(drive, share, false, user);
	}
	catch(e)
	{
		return 0;
	}
  return 1;
}