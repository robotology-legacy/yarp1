#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <math.h>
#include <string.h>

#include "tube.h"
#include "input.h"
#include "output.h"
#include "structs.h"

#include "listen.h"

//#define SHARK

/*  BOOLEAN CONSTANTS  */
#define FALSE                     0
#define TRUE                      1

/*  COMMAND LINE ARGUMENT VARIABLES  */
int verbose = FALSE;
int remote = FALSE;

void printInfo(struct _TRMData *data, char *inputFile);

/******************************************************************************
*
*	function:	printInfo
*
*	purpose:	Prints pertinent variables to standard output.
*
*       arguments:      none
*
*	internal
*	functions:	glotPitchAt, glotVolAt, aspVolAt, fricVolAt,
*                       fricPosAt, fricCFAt, fricBWAt, radiusAtRegion,
*                       velumAt
*
*	library
*	functions:	printf
*
******************************************************************************/

void printInfo(struct _TRMData *data, char *inputFile)
{
#if 0
    int i;

    /*  PRINT INPUT FILE NAME  */
    printf("input file:\t\t%s\n\n", inputFile);

    /*  ECHO INPUT PARAMETERS  */
    printf("outputFileFormat:\t");
    if (data->inputParameters.outputFileFormat == AU_FILE_FORMAT)
        printf("AU\n");
    else if (data->inputParameters.outputFileFormat == AIFF_FILE_FORMAT)
        printf("AIFF\n");
    else if (data->inputParameters.outputFileFormat == WAVE_FILE_FORMAT)
        printf("WAVE\n");

    printf("outputRate:\t\t%.1f Hz\n", data->inputParameters.outputRate);
    printf("controlRate:\t\t%.2f Hz\n\n", data->inputParameters.controlRate);

    printf("volume:\t\t\t%.2f dB\n", data->inputParameters.volume);
    printf("channels:\t\t%-d\n", data->inputParameters.channels);
    printf("balance:\t\t%+1.2f\n\n", data->inputParameters.balance);

    printf("waveform:\t\t");
    if (data->inputParameters.waveform == PULSE)
	printf("pulse\n");
    else if (data->inputParameters.waveform == SINE)
	printf("sine\n");
    printf("tp:\t\t\t%.2f%%\n", data->inputParameters.tp);
    printf("tnMin:\t\t\t%.2f%%\n", data->inputParameters.tnMin);
    printf("tnMax:\t\t\t%.2f%%\n", data->inputParameters.tnMax);
    printf("breathiness:\t\t%.2f%%\n\n", data->inputParameters.breathiness);

    printf("nominal tube length:\t%.2f cm\n", data->inputParameters.length);
    printf("temperature:\t\t%.2f degrees C\n", data->inputParameters.temperature);
    printf("lossFactor:\t\t%.2f%%\n\n", data->inputParameters.lossFactor);

    printf("apScale:\t\t%.2f cm\n", data->inputParameters.apScale);
    printf("mouthCoef:\t\t%.1f Hz\n", data->inputParameters.mouthCoef);
    printf("noseCoef:\t\t%.1f Hz\n\n", data->inputParameters.noseCoef);

    for (i = 1; i < TOTAL_NASAL_SECTIONS; i++)
	printf("n%-d:\t\t\t%.2f cm\n", i, data->inputParameters.noseRadius[i]);

    printf("\nthroatCutoff:\t\t%.1f Hz\n", data->inputParameters.throatCutoff);
    printf("throatVol:\t\t%.2f dB\n\n", data->inputParameters.throatVol);

    printf("modulation:\t\t");
    if (data->inputParameters.modulation)
	printf("on\n");
    else
	printf("off\n");
    printf("mixOffset:\t\t%.2f dB\n\n", data->inputParameters.mixOffset);

    /*  PRINT OUT DERIVED VALUES  */
    printf("\nactual tube length:\t%.4f cm\n", actualTubeLength);
    printf("internal sample rate:\t%-d Hz\n", sampleRate);
    printf("control period:\t\t%-d samples (%.4f seconds)\n\n",
	   controlPeriod, (float)controlPeriod/(float)sampleRate);

#if DEBUG
    /*  PRINT OUT WAVE TABLE VALUES  */
    printf("\n");
    for (i = 0; i < TABLE_LENGTH; i++)
	printf("table[%-d] = %.4f\n", i, wavetable[i]);
#endif

    printControlRateInputTable(data);
#endif
}

/******************************************************************************
*
*	function:	main
*
*	purpose:	Controls overall execution.
*
*       arguments:      inputFile, outputFile
*
*	internal
*	functions:	parseInputFile, initializeSynthesizer, printInfo,
*                       synthesize, writeOutputToFile
*
*	library
*	functions:	strcpy, fprintf, exit, printf, fflush
*
******************************************************************************/

int main(int argc, char *argv[])
{
  printf("init_listen starts\n");
  init_listen();
  printf("init_listen stops\n");

    char inputFile[MAXPATHLEN + 1];
    char outputFile[MAXPATHLEN + 1];
    TRMData *inputData;
    TRMTubeModel *tube;

    /*  PARSE THE COMMAND LINE  */
    if (argc == 3) {
	strcpy(inputFile, argv[1]);
	strcpy(outputFile, argv[2]);
    } else if ((argc == 4) && (!strcmp("-v", argv[1]))) {
	verbose = TRUE;
	strcpy(inputFile, argv[2]);
	strcpy(outputFile, argv[3]);
    } else if ((argc == 4) && (!strcmp("-r", argv[1]))) {
	remote = TRUE;
	strcpy(inputFile, argv[2]);
	strcpy(outputFile, argv[3]);
    } else {
	fprintf(stderr, "Usage:  %s [-v|-r] inputFile outputFile\n", argv[0]);
	exit(-1);
    }

#ifdef SHARK
    {
        char buf[100];
        printf("Waiting to start...\n");
        gets(buf);
    }
#endif

    /*  PARSE THE INPUT FILE FOR INPUT INFORMATION  */
    inputData = parseInputFile(inputFile);
    if (inputData == NULL) {
	fprintf(stderr, "Aborting...\n");
	exit(-1);
    }

    /*  INITIALIZE THE SYNTHESIZER  */
    tube = TRMTubeModelCreate(&(inputData->inputParameters));
    if (tube == NULL) {
	fprintf(stderr, "Aborting...\n");
	exit(-1);
    }

    /*  PRINT OUT PARAMETER INFORMATION  */
    if (verbose)
	printInfo(inputData, inputFile);

    /*  PRINT OUT CALCULATING MESSAGE  */
    if (verbose) {
	printf("\nCalculating floating point samples...");
	fflush(stdout);
    }

    /*  SYNTHESIZE THE SPEECH  */
    if (verbose) {
	printf("\nStarting synthesis\n");
	fflush(stdout);
    }
    synthesize(tube, inputData);

    /*  PRINT OUT DONE MESSAGE  */
    if (verbose)
	printf("done.\n");

    /*  OUTPUT SAMPLES TO OUTPUT FILE  */
    writeOutputToFile(&(tube->sampleRateConverter), inputData, outputFile);

    /*  PRINT OUT FINISHED MESSAGE  */
    if (verbose)
	printf("\nWrote scaled samples to file:  %s\n", outputFile);

    TRMTubeModelFree(tube);

#ifdef SHARK
    {
        char buf[100];
        printf("Done, waiting...\n");
        gets(buf);
    }
#endif

    return 0;
}
