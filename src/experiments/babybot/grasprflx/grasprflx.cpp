// grasprflx.cpp : Defines the entry point for the console application.
//

#include "reflexstates.h"
#include <YARPConfigFile.h>
#include <YARPParseParameters.h>

const int N = 20;
const int _nJoints = 6;

double _threshold = 0.0;
int _nSynergies = 0;
const YARPString __defaultCfgFile = "grasprflx.ini";

void readConfigFile(const YARPString &filename, ReflexShared &rflxShared)
{
	YARPConfigFile file;

	char *root = GetYarpRoot();
	char path[256];

	ACE_OS::sprintf (path, "%s/conf/babybot/", root); 

	file.set(path, filename);
	file.get("[GRASPREFLEX]", "Threshold", &_threshold);
	file.get("[SYNERGIES]", "N", &_nSynergies);

	// alloc commands
	YVector *_closePos = new YVector [_nSynergies];
	YVector *_openPos = new YVector [_nSynergies];

	int i = 0;
	for(i = 0; i < _nSynergies; i++)
	{
		char tmp1[80];
		char tmp2[80];

		sprintf(tmp1, "Open%d", i);
		sprintf(tmp2, "Close%d", i);

		_closePos[i].Resize(_nJoints);
		_openPos[i].Resize(_nJoints);
		
		file.get("[SYNERGIES]", tmp1, _openPos[i].data(), _nJoints);
		file.get("[SYNERGIES]", tmp2, _closePos[i].data(), _nJoints);

		_openPos[i] = _openPos[i]*degToRad;
		_closePos[i] = _closePos[i]*degToRad;
	}

	rflxShared.set(_nSynergies, _openPos, _closePos);

	delete [] _closePos;
	delete [] _openPos;
}

int main(int argc, char* argv[])
{
	bool norelease = false;
	if (YARPParseParameters::parse(argc, argv, "norelease"))
		norelease = true;
	YARPString cfgFile;
	if (!YARPParseParameters::parse(argc, argv, "cfg", cfgFile))
		cfgFile = __defaultCfgFile;

	ReflexShared shared;
	GRBehavior behavior(&shared);
	GRBLoopTouch loopTouch;
	GRBWaitIdle waitClose1;
	GRBWaitIdle waitOpen1;
	GRBWaitIdle waitOpen2;
	GRBPickRndAction pickRnd;

	GRBWaitDeltaT waitT[] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2,
							 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 };
	GRBGrasp grasp;
	GRBCloseOutputCommand closeCmd;
	GRBOpenOutputCommand openCmd;
	GRBOutputSignal	sendClutching(YBVGraspRflxClutch);
	GRBInit init;
	GRBSimpleInput motionDone(YBVHandDone);
	GRBSimpleInput forceOpen(YBVGraspRflxForceOpen);

	readConfigFile(cfgFile, shared);

	behavior.setInitialState(&loopTouch);
	behavior.add(&init, &loopTouch, &pickRnd);
	behavior.add(NULL, &loopTouch, &loopTouch);	// loopTouch is a blocking state
	behavior.add(NULL, &pickRnd, &waitOpen1, &openCmd);
	behavior.add(&motionDone, &waitOpen1, &grasp, &closeCmd);
	behavior.add(NULL, &grasp, &waitClose1);
	behavior.add(&motionDone, &waitClose1, &waitT[0], &sendClutching);
	for(int i = 0; i<(N-1); i++)
		behavior.add(NULL, &waitT[i], &waitT[i+1]);

	// check no automatic release
	if (norelease)
		behavior.add(&forceOpen, &waitT[N-1], &waitOpen2, &openCmd);
	else
		behavior.add(NULL, &waitT[N-1], &waitOpen2, &openCmd);
	
	behavior.add(&motionDone, &waitOpen2, &loopTouch);
	
	behavior.Begin();
	behavior.loop();

	return 0;
}
