// #include "Rf.h"
#include <yarp/RFNet.h>
#include <yarp/utils.h>
#include <iostream>
#include <vector>
#include <list>

struct Error
{
	double av;
	double std;
};

// void GetInput2(std::istream & device, YVector& X, YVector& Y);
void readDataSet(const char *filename, std::vector<YVector> &v);
Error testRun(RFNet &net, std::vector<YVector> &testSet);
std::list<YVector> testRun2(RFNet &net, std::vector<YVector> &testSet);
void split(const YVector &in, YVector &X, YVector &Y);
void dump(const char *filename, const std::list<YVector> res);
void dump(const char *filename, const std::list<double> res);
void dump(const char *filename, const std::list<Error> res);
void append(const std::list<YVector> &in, std::list<YVector> &out);

using namespace std;

// const char offlineFile[] = "july04Offline.txt";
// const char onlineFile[] = "july04Online.txt";
// const char dataFile[] = "july04-2Dp.dat";
// const char iniNetFile[] = "reaching.ini";
// const char outNetFile[] = "reachingOut.ini";

const char trainFile[] = "rnd.train";
const char testFile[] = "rnd.test";
const char tsFullFile[] = "testSetFull.dat";
const char tsErrorFile[] = "testSetError.dat";
const char olErrorFile[] = "online.dat";
const char inverseNetFile[] = "inverse.ini";
const char directNetFile[] = "direct.ini";
const char inverseOutNetFile[] = "inverseOut.ini";
const char directOutNetFile[] = "directOut.ini";

std::vector<YVector> trainSet;
std::vector<YVector> testSet;
std::list<YVector> testResults;
std::vector<int> blockSize;
std::list<double> testOnline;
std::list<Error> testSetError;

int main()
{
	YVector Yp(3),X(3),Y(3),Xp(3);
			
	RFNet inverse;
	RFNet direct;
	
	// inizializzazione automatica da file 

	int a = inverse.LoadNet(inverseNetFile,"",1);
	
	if (a == YARP_FAIL )
	{ 
		cout << "Problemi con il file "<< endl;
		return 1;
	}
	
	//////////// train set
	readDataSet(trainFile, trainSet);
	//////////// test set
	readDataSet(testFile, testSet);
	
	// direct model
	int batchSize = 20;
	int lastBatch;
	int K = trainSet.size()/batchSize;
	lastBatch = batchSize+trainSet.size()%batchSize;
	int block = 0;
	for(block = 0; block<K-1; block++)
		blockSize.push_back(batchSize);
	blockSize.push_back(lastBatch);

	printf("Train set is %d batch size is %d\n", trainSet.size(), batchSize);
	printf("Splitting train set in %dx%d + %d\n", K-1, batchSize, lastBatch);
		

	int i = 0;
	int index = 0;
	double vecw;
	FILE *errorFP = fopen("dump.txt", "w");
	for (block = 0; block<K; block++)
	{
		testSetError.push_back(testRun(inverse, testSet));
		
		// perform test set simulation
		for(i=0; i<blockSize[block]; i++)
		{
			index = block*batchSize + i;
		//	printf("Training with %dth sample\n", index+1);
			split(trainSet[index], X, Y);
			
			// simulate first
			inverse.Simulate(X, 0.001, Yp);
			testOnline.push_back((Yp-Y).norm2());

			// train
			vecw = inverse.Train (X,Y,Yp);
		}
		
		std::list<YVector> res = testRun2(inverse, testSet);
		append(res,testResults);
	}
	
	testSetError.push_back(testRun(inverse, testSet));
	
	dump(tsFullFile, testResults);
	dump(olErrorFile, testOnline);
	dump(tsErrorFile, testSetError);
		
	// inverse.SaveNet (inverseOutNetFile,"");
	/////////////////
/*
	// direct
	a = direct.LoadNet(directNetFile,"",1);
	
	if (a == YARP_FAIL )
	{ 
		cout << "Problemi con il file "<< endl;
		return 1;
	}
	output.open(outFile2); 
	input2.open(trainFile);
		
	GetInput2(input2,X,Y);

	readDataSet();

	ns=0;
	// direct
	while(1) {

		vecw = direct.Train (Y,X,Xp);

		GetInput2(input2,X,Y);

		if (input2.eof ()) break;

		direct.Simulate (Y,0.001,Xp);

		YVector tmp;
		tmp = Xp-X;
		output << tmp(1) << '\t';
		output << tmp(2) << '\t';
		output << tmp(3) << '\n';
		ns++;
	}
	
	cout << "Read " << ns << " samples\n";
	output.close(); 
	input.close();
	direct.SaveNet (directOutNetFile,"");
*/
/*
	/// TEST
	output2.open(offlineFile); 
	input2.open(dataFile);
	
	GetInput(input2,X,Y);

	while(1) {
		GetInput(input2,X,Y);

		if (input2.eof ()) break;
		prima.Simulate (X,0.001,Yp);

		YVector tmp;
		tmp = Yp-Y;
		output2 << tmp.norm2() << '\n';
	}
	
	output2.close(); 
	input2.close();
	////////////////////
	*/
	return 0;
}

void readDataSet(const char *filename, std::vector<YVector> &v)
{
	ifstream input;
	input.open(filename);

	v.clear();	//just in case
	
	while(!input.eof()) 
	{
		YVector tmp(6);
		int k = 1;
		for(k=1; k<=6; k++)
			input >> tmp(k);

		v.push_back(tmp);
	}
	input.close();

	printf("%s read %d examples\n", filename, v.size());
}

Error stat(const std::vector<double> &v);

Error testRun(RFNet &net, std::vector<YVector> &testSet)
{
	double err = 0;

	std::vector<double> errors;

	for(int i = 0; i < testSet.size(); i++)
	{
		YVector X(3);
		YVector Y(3);
		YVector Yp(3);

		split(testSet[i], X, Y);

		net.Simulate(X,0.001,Yp);

		YVector tmp;
		tmp = Yp-Y;
		// cout<<Yp(1)<<"\n";
		err = tmp.norm2();
	//	if (err < 1)
			errors.push_back(err);
	//	else
	//		cout << "removed\n";
	}

	return stat(errors);
}

std::list<YVector> testRun2(RFNet &net, std::vector<YVector> &testSet)
{
	std::list<YVector> ret;
	for(int i = 0; i < testSet.size(); i++)
	{
		YVector X(3);
		YVector Y(3);
		YVector Yp(3);

		split(testSet[i], X, Y);

		net.Simulate(X,0.001,Yp);

		YVector tmp;
		tmp = Yp-Y;
		ret.push_back(tmp);
	}

	return ret;
}

Error stat(const std::vector<double> &v)
{
	Error err;
	err.av = 0.0;
	err.std = 0.0;
	for (int i = 0; i < v.size();i++)
		err.av+=v[i]/v.size();
	
	for (i = 0; i < v.size();i++)
		err.std+= (v[i]-err.av)*(v[i]-err.av);

	err.std /= v.size();
	err.std=sqrt(err.std);
	return err;
}

void split(const YVector &in, YVector &X, YVector &Y)
{
	int k = 1;
	int m = 1;
	for(k=1; k<=X.Length();k++)
	{
		X(k) = in(m);
		m++;
	}

	for(k=1; k<=Y.Length();k++)
	{
		Y(k) = in(m);
		m++;
	}
}

void dump(const char *filename, const std::list<YVector> res)
{
	ofstream of(filename);
	std::list<YVector>::const_iterator i = res.begin();
	for(; i != res.end(); i++)
	{
		
		for(int k = 1; k <= (*i).Length(); k++)
			of << ((*i)(k)) << " ";
		of << "\n";
	}

	of.close();
}

void dump(const char *filename, const std::list<Error> res)
{
	ofstream of(filename);
	std::list<Error>::const_iterator i = res.begin();
	for(; i != res.end(); i++)
		of << (*i).av << "\t" << (*i).std << "\n";
		
	of.close();
}

void dump(const char *filename, const std::list<double> res)
{
	ofstream of(filename);
	std::list<double>::const_iterator i = res.begin();
	for(; i != res.end(); i++)
			of << (*i) << "\n";
	
	of.close();
}

void append(const std::list<YVector> &in, std::list<YVector> &out)
{
	std::list<YVector>::const_iterator i = in.begin();
	for(; i != in.end(); i++)
		out.push_back(*i);
}
