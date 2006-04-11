// BodyMap.js
//
// example use of a learner. BodyMap is connected to mirrorCollector (for
// collecting data from the setup) and to a learner (for predicting)
//
// collector MUST run on atlas with tracker and cameras on ONLY.
//

// create instance of JScript
var WshShell = WScript.CreateObject("WScript.Shell");

// launch mirrorCollector, learner and BodyMap
WshShell.Run ("on -d -n atlas \"mirrorCollector\"", 7, false);
WshShell.Run ("on -d -n atlas \"learner --dom 3 --cod 4 --ex 500 --u .2 .2 .2\"", 1, false);
//WshShell.Run ("on -d -n oceanus \"learner --dom 3 --cod 4 --ex 500\"", 1, false);
WshShell.Run ("on -d -n atlas \"BodyMap\"", 1, false);
