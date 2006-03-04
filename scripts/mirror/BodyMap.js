// BodyMap.js
//
// example use of a learner. BodyMap is connected to mirrorCollector (for
// collecting data from the setup) and to a learner (for predicting)
//
// collector MUST run on atlas with tracker and cameras on ONLY.
//

// create instance of JScript
var WshShell = WScript.CreateObject("WScript.Shell");

// launch two mirrorCollector, learner and BodyMap
WshShell.Run ("on -d -n atlas mirrorCollector", 1, false);
WshShell.Run ("learner --dom 3 --cod 4 --ex 150", 1, false);
WshShell.Run ("BodyMap", 1, false);
