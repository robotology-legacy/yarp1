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


#define N_IMAGES 10
#define PI 3.1415926535

#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )

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

int main(int argc, char* argv[])
{
	YARPGrabber	grabber;
	int boardN = 0;
	int sizeX = 640;
	int sizeY = 480;
	int ret = 0;
	int rhoSize = 256;
	int thetaSize = 180;
	unsigned char *buffer = NULL;
	YARPImageOf<YarpPixelBGR> img_buffer[N_IMAGES];
	YARPImageOf<YarpPixelMono> susan_img;
	YARPImageOf<YarpPixelMono> hough_img;
	double initialOrientation;
	double finalOrientation;
	double difference;
	YARPHoughTransform houghFlt;
	YARPSusanFilter flt;
	double angle;
	double acc;

	printf("\n[eyeCalib] Allocating images and filter (%d x %d)...\n", sizeX, sizeY);
	for (int i=0; i<N_IMAGES; i++)
		img_buffer[i].Resize (sizeX, sizeY);
	susan_img.Resize (sizeX, sizeY);
	flt.resize(sizeX, sizeY);
	printf("[eyeCalib] Setting up the sensor..\n");
	ret = grabber.initialize(boardN, sizeX, sizeY);
	if (ret == YARP_FAIL)
	{
		printf("[eyeCalib] ERROR in _grabber.initialize(). Quitting.\n");
		exit (1);
	}
	printf("[eyeCalib] Allocating Hough Filter (theta %d, rho %d)..\n", thetaSize, rhoSize);
	houghFlt.resize(thetaSize, rhoSize);

	printf("[eyeCalib] Hit any key to start calibration..");
	char c;
	c = getch();
	printf("\n[eyeCalib] Grabbing frames (1st cycle)\n");
	for (i=0; i<N_IMAGES; i++)
	{
		ret = grabber.waitOnNewFrame ();
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in grabber.waitOnNewFrame(). Quitting.\n");
			exit (1);
		}

		ret = grabber.acquireBuffer(&buffer);
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in grabber.acquireBuffer(). Quitting.\n");
			exit (1);
		}
		
		memcpy((unsigned char *)img_buffer[i].GetRawBuffer(), buffer, sizeX * sizeY * 3);

		ret = grabber.releaseBuffer ();
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in grabber.releaseBuffer(). Quitting.\n");
			exit (1);
		}
		
		printf(".");
	}
	
	printf("\n[eyeCalib] Calculating main orientation (1st cycle)\n");
	acc = 0.0;
	for (i=0; i<N_IMAGES; i++)
	{
		flt.apply(&(img_buffer[i]), &susan_img);
		houghFlt.apply(&susan_img);
		houghFlt.getMaxTheta(&angle);
		acc += _normalize0ToPI(PI/2.0-angle);
		printf(".");
	}
	initialOrientation = acc / N_IMAGES;
	
	printf("\n[eyeCalib] Main orientation is %.3frad (%.3fdeg) - variance %.2f\n", initialOrientation, _rad2deg(initialOrientation));

	printf("[eyeCalib] Hit any key to complete calibration..\n");
	c = getch();
	printf("[eyeCalib] Grabbing frames (2nd cycle)\n");
	for (i=0; i<N_IMAGES; i++)
	{
		ret = grabber.waitOnNewFrame ();
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in grabber.waitOnNewFrame(). Quitting.\n");
			exit (1);
		}

		ret = grabber.acquireBuffer(&buffer);
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in grabber.acquireBuffer(). Quitting.\n");
			exit (1);
		}
		
		memcpy((unsigned char *)img_buffer[i].GetRawBuffer(), buffer, sizeX * sizeY * 3);

		ret = grabber.releaseBuffer ();
		if (ret == YARP_FAIL)
		{
			printf("[eyeCalib] ERROR in grabber.releaseBuffer(). Quitting.\n");
			exit (1);
		}
		
		printf(".");
	}

	printf("\n[eyeCalib] Calculating main orientation (2nd cycle)\n");
	acc = 0.0;
	for (i=0; i<N_IMAGES; i++)
	{
		flt.apply(&(img_buffer[i]), &susan_img);
		houghFlt.apply(&susan_img);
		houghFlt.getMaxTheta(&angle);
		acc += _normalize0ToPI(PI/2.0-angle);
		printf(".");
	}
	finalOrientation = acc / N_IMAGES;
	
	printf("\n[eyeCalib] Main orientation is %.3frad (%.3fdeg)\n", finalOrientation, _rad2deg(finalOrientation));

	difference = finalOrientation - initialOrientation;
	printf("[eyeCalib] Difference is %.3frad (%.3fdeg)\n", difference, _rad2deg(difference));
	
	printf("\n[eyeCalib] Do you want to save results to file (Y/n)?");
	c = getch();
	if (c != 'n')
	{
		char fileName[255];
		FILE *outFile;
		printf("\n[eyeCalib] File name ? ");
		scanf("%s", fileName);
		printf("[eyeCalib] Append data (Y/n)?");
		c = getch();
		if (c != 'n')
		{
			outFile = fopen(fileName,"w");
		}
		else
		{
			outFile = fopen(fileName,"a");
		}

		if (!outFile)
		{
			printf("[eyeCalib] ERROR opening the file %s. Saving aboorted.\n", fileName);
		}
		else
			fprintf(outFile, "%f;%f\n", initialOrientation, finalOrientation);

		fclose(outFile);
	}
	
	printf("[eyeCalib] Releasing the sensor..\n");
	ret = grabber.uninitialize();
	if (ret == YARP_FAIL)
	{
		printf("[eyeCalib] ERROR in grabber.uninitialize(). Quitting.\n");
		exit (1);
	}

	printf("[eyeCalib] Bye.\n");

	return 0;
}
