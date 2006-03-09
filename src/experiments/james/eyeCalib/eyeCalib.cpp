// eyeCalib.cpp : Defines the entry point for the console application.
//
//=============================================================================
// YARP Includes
//=============================================================================
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPRobotHardware.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include <conio.h>
#include <math.h>

//=============================================================================
// Local Includes
//=============================================================================
#include "YARPHoughTransform.h"
#include "YARPSusanFilter.h"


#define N_IMAGES 1
#define PI 3.1415926535

#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )

YARPGrabber	_grabber;
YARPImageOf<YarpPixelBGR> _imgBuffer[N_IMAGES];
int _sizeX = 640;
int _sizeY = 480;
YARPHoughTransform _houghFlt;
YARPSusanFilter _susanFlt;
YARPImageOf<YarpPixelMono> _susanImg;
// DEBUG
//YARPHead _head;
// END DEBUG
const char *const HEAD_INI_FILE = "head.ini";
double *_headJointsStore;
double _initialOrientation;
double _finalOrientation;
double orientation;
double distance;
int count = 0;
char FilePath[512];


double _normalize0ToPI(double angle)
{
	int k;
	double normalized;
	if (abs(angle) > PI)
	{
		k = angle/PI;
		normalized = angle - double(k*PI);
	}
	else 
		normalized = angle;
	if (angle < 0)
		normalized += PI;
	
	return normalized;
}

double _rad2deg(double rad)
{
	return double(rad*180.0/PI);
}

double _mean(double *data, int size)
{
	double mean = 0.0;
	for( int i=0; i<size; i++)
		mean += data[i];

	mean = mean / size;
	return mean;
}

double _variance(double mean, double *data, int size)
{
	double acc = 0.0;
	double variance;

	for (int i=0; i<size; i++)
		acc += pow((data[i] - mean),2);

	variance = sqrt((1/(1-size)) * acc);
	return variance;
}

void releaseSensor(void)
{
	printf("[eyeCalib] Releasing the sensor..\n");
	int ret = _grabber.uninitialize();
	if (ret == YARP_FAIL)
	{
		printf("[eyeCalib] ERROR in _grabber.uninitialize(). Quitting.\n");
		exit (1);
	}
}

void calibrate(void)
{
	
	unsigned char *buffer = NULL;
	int ret;
	printf("\n[eyeCalib] Grabbing frames (1st cycle)\n");
	for (int i=0; i<N_IMAGES; i++)
	{
		ret = _grabber.waitOnNewFrame ();
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in _grabber.waitOnNewFrame(). Quitting.\n");
			releaseSensor();
			exit (1);
		}

		ret = _grabber.acquireBuffer(&buffer);
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in _grabber.acquireBuffer(). Quitting.\n");
			releaseSensor();
			exit (1);
		}
	
		memcpy((unsigned char *)_imgBuffer[i].GetRawBuffer(), buffer, _sizeX * _sizeY * 3);

		ret = _grabber.releaseBuffer ();
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in _grabber.releaseBuffer(). Quitting.\n");
			releaseSensor();
			exit (1);
		}
	
		printf(".");
	}

	printf("\n[eyeCalib] Calculating main orientation (1st cycle)\n");
	double acc = 0.0;
	double acc2 = 0.0;
	double angle;
	double dist;
	char *root = GetYarpRoot();

	for (i=0; i<N_IMAGES; i++)
	{
		_susanFlt.apply(&(_imgBuffer[i]), &_susanImg);
		_houghFlt.apply(&_susanImg);
		_houghFlt.getMaxTheta(&angle);
		_houghFlt.getMaxRho(&dist);
		acc += _normalize0ToPI(PI/2.0-angle);
		acc2 += dist;

		printf(".");
		printf("%.2f acc", acc);

		ACE_OS::sprintf (FilePath, "%s%s%s%03u%s\0", root, "/src/experiments/james/eyeCalib/Figure/","Img", count, ".pgm"); 
		YARPImageFile::Write(FilePath,_imgBuffer[i],1);
		count += 1;
	}
	orientation = acc / N_IMAGES;
	distance = acc2 / N_IMAGES;

}

bool readLine(FILE *file, double *vel, double *posArray)
{
	char line[255];
	double posJ0, posJ1, posJ2, posJ3;
	do
	{
		if (feof(file))
			return false;
		//fscanf(file,"%s\n",line);
		fgets(line,255,file);
	}
	while (line[0] == '#');
	sscanf(line, "%lf;%lf;%lf;%lf;%lf",vel, &posJ0, &posJ1, &posJ2, &posJ3);
	posArray[0] = posJ0;
	posArray[1] = posJ1;
	posArray[2] = posJ2;
	posArray[3] = posJ3;
	return true;
}

void moveHead(double vel, double *posVector)
{
// DEBUG
	/*
	for( int j=0;j<4;j++)
		headJointsStore[j] = posVector[j];
	_head.setPositions (headJointsStore);
	for(j=0;j<4;j++)
		headJointsStore[j] = vel;
	_head.setVelocities (headJointsStore);
	*/
// END DEBUG
}

void initializeHead(void)
{
	// HEAD INITIALIZATION
	char *root = GetYarpRoot();
	char path[512];
	int nj;
	ACE_OS::sprintf (path, "%s/%s\0", root, ConfigFilePath); 
	// initialize head controller on bus 1.
// DEBUG
	/*
	YARPRobotcubHeadParameters parameters;
	parameters.load (YARPString(path), YARPString(HEAD_INI_FILE));
	parameters._message_filter = 20;
	parameters._p = printf;

	int ret = _head.initialize(parameters);
	if (ret != YARP_OK)
	{
		printf("[eyeCalib] ERROR: Can't start the interface with the robot head, please make sure the hardware and control cards are on.\nThe ini file was: %s%s", path, HEAD_INI_FILE);
		exit(1);
	}
	nj = _head.nj();
	*/
// END DEBUG
	nj = 4;
	_headJointsStore = new double[nj];
	for (int j=0;j<nj;j++)
		_headJointsStore[j] = 0.0;
}

void releaseHead(void)
{
	delete [] _headJointsStore;
}

int main(int argc, char* argv[])
{
	
	int boardN = 0;
	int ret = 0;
	int rhoSize = 256;
	int thetaSize = 720;
	
	YARPImageOf<YarpPixelMono> hough_img;
	char c;

	printf("\n[eyeCalib] Setting up the sensor..");
	ret = _grabber.initialize(boardN, _sizeX, _sizeY);
	if (ret == YARP_FAIL)
	{
		printf("[eyeCalib] ERROR in _grabber.initialize(). Quitting.\n");
		exit (1);
	}

	initializeHead();

	printf("\n[eyeCalib] Allocating images and filter (%d x %d)...", _sizeX, _sizeY);
	for (int i=0; i<N_IMAGES; i++)
		_imgBuffer[i].Resize (_sizeX, _sizeY);
	_susanImg.Resize (_sizeX, _sizeY);
	_susanFlt.resize(_sizeX, _sizeY);
	printf("\n[eyeCalib] Allocating Hough Filter (theta %d, rho %d)..", thetaSize, rhoSize);
	_houghFlt.resize(thetaSize, rhoSize);
	char fileName[255];
	FILE *outFile = NULL;
	double posVector[4];
	double vel;
	printf("\n[eyeCalib] Do you want to save results to file ? [Y/n] ");
	c = getch();
	if (c != 'n')
	{
		
		printf("\n[eyeCalib] File name ? ");
		scanf("%s", fileName);
		printf("[eyeCalib] Append data (Y/n)?");
		c = getch();
		if (c == 'n')
		{
			outFile = fopen(fileName,"w");
			fprintf(outFile, "velocity;J0;J1;J2;J3;orientation\n");
		}
		else
		{
			outFile = fopen(fileName,"a");
		}

		if (!outFile)
		{
			printf("[eyeCalib] ERROR opening the file %s. Saving aboorted.\n", fileName);
		}
	}
	printf("\n[eyeCalib] Interactive calibration or Load data form file? [I/l] ");
	c = getch();
	if ( c == 'l')
	{
		printf("\n[eyeCalib] File to load? ");
		scanf("%s", fileName);
		FILE *dataFile = NULL;
		dataFile = fopen(fileName,"r");
		if (dataFile == NULL)
		{
			printf("[eyeCalib] ERROR opening the file %s. Quitting.\n", fileName);
			releaseSensor();
			if (outFile != NULL)
				fclose(outFile);
			exit(1);
		}
		bool ret = false;
		while (readLine(dataFile, &vel, posVector) == true)
		{
			printf("\n[eyeCalib] Moving [%.2lf %.2lf %.2lf %.2lf * %.2lf]..", posVector[0], posVector[1], posVector[2], posVector[3], vel);
			printf("\n[eyeCalib] Hit any key when Position has been reached..");
			moveHead(vel,posVector);
			c = getch();
			calibrate();
			printf("\n[eyeCalib] Main orientation is %.3frad (%.3fdeg) - variance %.2f", orientation, _rad2deg(orientation));
			if (outFile != NULL)
				fprintf(outFile, "%lf;%lf;%lf;%lf;%lf;%lf\n", vel, posVector[0], posVector[1], posVector[2], posVector[3], orientation);
		}
		fclose(dataFile);
	}
	else
	{
		do
		{
			printf("\n[eyeCalib] Avaible Joints:\n\t0 - Left Pan\n\t1 - Left Tilt\n\t2 - Right Pan\n\t3 - Left Tilt");
			printf("\n[EyeCalib] Move to? [J0;J1;J2;J3] ");
			scanf("%lf;%lf;%lf;%lf", &(posVector[0]), &(posVector[1]), &(posVector[2]), &(posVector[3]));
			printf("\n[eyeCalib] Move with velocity? ");
			scanf("%lf", &vel);
			printf("\n[eyeCalib] Moving [%.2lf %.2lf %.2lf %.2lf * %.2lf]..", posVector[0], posVector[1], posVector[2], posVector[3], vel);
			printf("\n[eyeCalib] Hit any key when Position has been reached..");
			moveHead(vel, posVector);
			c = getch();
			calibrate();
			printf("\n[eyeCalib] Main orientation is %.3frad (%.3fdeg) - variance %.2f", orientation, _rad2deg(orientation));
			printf("\n[eyeCalib] Main distance is %.3fpixels", distance);
			if (outFile != NULL)
				fprintf(outFile, "%lf;%lf;%lf;%lf;%lf;%lf;%lf\n", vel, posVector[0], posVector[1], posVector[2], posVector[3], orientation, distance);
			printf("\n[eyeCalib] Another loop ? [Y/n] ");
			c = getch();
		} while (c != 'n');
	}

	if (outFile != NULL)
			fclose(outFile);
	releaseSensor();
	releaseHead();

	printf("\n[eyeCalib] Bye.\n");

	return 0;
}
