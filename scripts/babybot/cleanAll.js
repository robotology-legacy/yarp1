// Parse namer.conf, extract the list of nodes and run cleaner.js on each machine
// cleaner.js delete all files which match MEM_Acceptor*.* from tmp folders 
// (this script is used to clean shared memory temp files)
// July 2004, by nat

var WshShell = WScript.CreateObject("WScript.Shell");

var confName = "%YARP_ROOT%\\conf\\" + "namer.conf";
var expandedConfFile = WshShell.ExpandEnvironmentStrings(confName);
var machineList = new Array();
machineList = BuildComputerList(expandedConfFile);

// WScript.Echo("Found "+ machineList.length + " nodes");

var i;
for(i = 0; i < machineList.length; i++)
{
  // WScript.Echo("Cleaning: " + machineList[i]);
  WshShell.Run ("on -d -n "+ machineList[i]+ " cleaner.js", 7,true);
}

function BuildComputerList(filename)
{
  var filesystem = new ActiveXObject("Scripting.FileSystemObject");
  var inText = filesystem.OpenTextFile(filename, 1, true);

  var ret = new Array();

  // begin parse
  var i = 0;
  while(!inText.atEndOfStream)
	{
  	   var line = inText.ReadLine();
	   var Key = line.slice(0, 6);
	   if (Key == 'Node= ')
           {
		var nodeName = goesUntilSpace(line.slice(6));
	    	// WScript.Echo("Found a new node "+ nodeName);
		ret[i] = nodeName;
		i = i +1;
           }
		
	}

  inText.Close();

  return ret;
}

function goesUntilSpace(line)
{
	var i = 0;
	var ret = new String;
	var index = line.indexOf("local", 1);
	ret = line.slice(0, index);

 	return ret;
}