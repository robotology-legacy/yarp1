// remotelearnclient.cpp : Defines the entry point for the console application.
//

#include <conf/YARPConfig.h>
#include <wide_nameloc.h>

#include <YARPBPNNet.h>

#include <iostream>
#include <time.h>

#include <YARPParseParameters.h>
#include <YARPConfigFile.h>
#include <YARPBottle.h>
#include <YARPPort.h>

#include <YARPBabybotHeadKin.h>

using namespace std;

void parsePatternsCenters(double *, double *, double **, double **, int);
void parsePatternsParams(double *, double *, double **, double **, int);
void train(int argc, char *argv[]);
int read_set (const char *filename, double **in, double **t, int inSize, int outSize);
void help();

const char __portName[] = "/nnet/o";

int main(int argc, char* argv[])
{
	if ( YARPParseParameters::parse(argc, argv, "set") )
		train(argc, argv);
	else
		help();

	return 0;
}

void train(int argc, char *argv[])
{
	char trainFile[255];
	YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	_outPort.Register(__portName);
		
	YARPParseParameters::parse(argc, argv, "set", trainFile);
		
	double *input_train_set_raw;
	double *target_train_set_raw;
	int nPatterns = read_set(trainFile,
							&input_train_set_raw,
							&target_train_set_raw,
							11,
							5);
	
	double *input_train_set;
	double *target_train_set;
	parsePatternsCenters(input_train_set_raw, target_train_set_raw, &input_train_set, &target_train_set, nPatterns);

	ACE_OS::printf("Please connect me...\n");
	char c;
	cin >> c;

	// start sending stuff to remote learner
	int i,j;
	// for each pattern
	YARPBottle tmpBottle;
	double *tmpIn = input_train_set;
	double *tmpTarget = target_train_set;
	for(i = 0; i < nPatterns; i++)
	{
		// input
		for(j = 0; j < 3; j++)
		{
			tmpBottle.writeFloat(*tmpIn);
			tmpIn++;
		}
		
		// output
		for(j = 0; j < 3; j++)
		{
			tmpBottle.writeFloat(*tmpTarget);
			tmpTarget++;
		}

		_outPort.Content() = tmpBottle;
		ACE_OS::printf("Sending train sample #%d...", i);
		_outPort.Write(0);	// blocking send
		ACE_OS::printf("done !\n");
	}

	delete [] input_train_set;
	delete [] target_train_set;
	delete [] input_train_set_raw;
	delete [] target_train_set_raw;
}

void help()
{
	cout << "Use the following parameters to train the network:\n";
	cout << "-set train_file\n";
}
