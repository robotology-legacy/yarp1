// remotelearnclient.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPConfig.h>
// #include <wide_nameloc.h>

#include <yarp/YARPBPNNet.h>

#include <iostream>
#include <time.h>

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBPNNet.h>

#include <yarp/YARPBabybotHeadKin.h>

const int __nInput = 3;
const int __nOutput = 3;

using namespace std;

void parsePatternsCenters(double *, double *, double **, double **, int);
void parsePatternsParams(double *, double *, double **, double **, int);
void train(int argc, char *argv[]);
int read_set (const char *filename, double **in, double **t, int inSize, int outSize);
void help();

const char __outputPortName[] = "/client/nnet/o";
const char __inputPortName[] = "/client/nnet/i";

int main(int argc, char* argv[])
{
	if ( YARPParseParameters::parse(argc, argv, "-set") )
		train(argc, argv);
	else
		help();

	return 0;
}

void train(int argc, char *argv[])
{
	char trainFile[255];
	YARPOutputPortOf<YARPBabyBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
	_outPort.Register(__outputPortName);
	YARPInputPortOf<YARPBabyBottle> _inPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
	_inPort.Register(__inputPortName);
		
	YARPParseParameters::parse(argc, argv, "-set", trainFile);
		
	double *input_train_set_raw;
	double *target_train_set_raw;
	int nPatterns = read_set(trainFile,
							&input_train_set_raw,
							&target_train_set_raw,
							__nInput, __nOutput);
	
	double *input_train_set = input_train_set_raw;
	double *target_train_set = target_train_set_raw;
	
	ACE_OS::printf("Train set from %s\n", trainFile);
	ACE_OS::printf("#input:%d #output:%d\n", __nInput, __nOutput);
	ACE_OS::printf("#samples:%d\n", nPatterns);

	ACE_OS::printf("Please connect me...\n");
	char c;
	cin >> c;

	// start sending stuff to remote learner
	int i,j;
	// for each pattern
	YARPBabyBottle tmpBottle;
	double *tmpIn = input_train_set;
	double *tmpTarget = target_train_set;
	for(i = 0; i < nPatterns; i++)
	{
		tmpBottle.setID("TrainSample");
		tmpBottle.reset();
		// input
		for(j = 0; j < __nInput; j++)
		{
			tmpBottle.writeFloat(*tmpIn);
			tmpIn++;
		}
		
		// output
		for(j = 0; j < __nOutput; j++)
		{
			tmpBottle.writeFloat(*tmpTarget);
			tmpTarget++;
		}
		
		_outPort.Content() = tmpBottle;
		ACE_OS::printf("Sending train sample #%d...", i+1);
		_outPort.Write(1);	// blocking send
		ACE_OS::printf("done !\n");
		Sleep(100);
	}

	// block to receive network back from remote program
	/*
	_inPort.Read();
	
	ACE_OS::printf("Received new trained nnet\n");
	YARPBPNNetState tmp;
	YARPBPNNet net;
	extract(_inPort.Content(), tmp);
	net.load(tmp);
	net.save("y:\\zgarbage\\temp.ini");
	*/

	// delete [] input_train_set;
	// delete [] target_train_set;
	delete [] input_train_set_raw;
	delete [] target_train_set_raw;
}

void help()
{
	cout << "Use the following parameters to train the network:\n";
	cout << "-set train_file\n";
}