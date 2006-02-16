// BodyMap.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BodyMap.h"
#include "BodyMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBodyMapApp

BEGIN_MESSAGE_MAP(CBodyMapApp, CWinApp)
	//{{AFX_MSG_MAP(CBodyMapApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBodyMapApp construction

CBodyMapApp::CBodyMapApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBodyMapApp object

CBodyMapApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBodyMapApp initialization

BOOL CBodyMapApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CBodyMapDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BodyMapLearningBlockStruct::BodyMapLearningBlockStruct()
{

	// learning block constructor

	// initialise basic data types
	x0 = y0 = x1 = y1 = 0;
	sampleCount = 0;
	model[0] = model[1] = model[2] = model[3] = NULL;

	// initialise SVM paramters
	param.svm_type = EPSILON_SVR;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 1/3.0;      // 1/k
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 10;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	// initialise problems. sample buffer is unique,
	// and so is the number and dimension of the samples.
	prob[0].l = prob[1].l = prob[2].l = prob[3].l = numOfSamples;
	// allocate space for the values
	prob[0].y = (double*) calloc( 1, sizeof(double)*numOfSamples );
	prob[1].y = (double*) calloc( 1, sizeof(double)*numOfSamples );
	prob[2].y = (double*) calloc( 1, sizeof(double)*numOfSamples );
	prob[3].y = (double*) calloc( 1, sizeof(double)*numOfSamples );
	// allocate space for the samples and link pointers to the problems
	sample = (struct svm_node**) calloc ( 1, sizeof(struct svm_node*)*numOfSamples );
	prob[0].x = prob[1].x = prob[2].x = prob[3].x = sample;
	for ( int i=0; i<numOfSamples; ++i ) {
		sample[i] = (struct svm_node*) calloc ( 1, sizeof(struct svm_node)*4 );
		prob[0].x[i] = prob[1].x[i] = prob[2].x[i] = prob[3].x[i] = sample[i];
		for ( int j=0; j<4; ++j ) {
			sample[i][j].index = j+1;
			sample[i][j].value = 0;
		}
		sample[i][3].index = -1;
	}

	// check paramters. problem is not required unless we use NU-SVC,
	// which is never the case for us.
	const char* error_msg;
	error_msg = svm_check_parameter( NULL, &param );
	if ( error_msg != NULL ) {
		exit(YARP_FAIL);
	}

	// train for the first time. all data are zero. necessary for
	// prediction before the first real data acquisition.
	train();

	return;

}

BodyMapLearningBlockStruct::~BodyMapLearningBlockStruct()
{

	// learning block destructor

    if ( model[0] != NULL ) {
		svm_destroy_model( model[0] );
	}
	if ( model[1] != NULL ) {
		svm_destroy_model( model[1] );
	}
	if ( model[2] != NULL ) {
		svm_destroy_model( model[2] );
	}
	if ( model[3] != NULL ) {
		svm_destroy_model( model[3] );
	}

	free( prob[0].y );
    free( prob[1].y );
    free( prob[2].y );
    free( prob[3].y );
    svm_destroy_param( &param );

}

int BodyMapLearningBlockStruct::addSample(double x, double y, double z, double x0, double y0, double x1, double y1)
{

	// add a new sample to our training set.

	// if the buffer is full, stop and return failure, but reset
	// counter, so that next time it will be ok
	if ( sampleCount == numOfSamples ) {
		sampleCount = 0;
		return YARP_FAIL;
	} else {
		// otherwise, store a new sample and go on
		sample[sampleCount][0].index = 1;  sample[sampleCount][0].value = x;
		sample[sampleCount][1].index = 2;  sample[sampleCount][1].value = y;
		sample[sampleCount][2].index = 3;  sample[sampleCount][2].value = z;
		sample[sampleCount][3].index = -1; sample[sampleCount][3].value = 0;

		prob[0].y[sampleCount] = x0;
		prob[1].y[sampleCount] = y0;
		prob[2].y[sampleCount] = x1;
		prob[3].y[sampleCount] = y1;
		
		sampleCount++;

		return YARP_OK;
	}

}

void BodyMapLearningBlockStruct::train()
{

	if ( model[0] != NULL ) {
		svm_destroy_model( model[0] );
	}
	if ( model[1] != NULL ) {
		svm_destroy_model( model[1] );
	}
	if ( model[2] != NULL ) {
		svm_destroy_model( model[2] );
	}
	if ( model[3] != NULL ) {
		svm_destroy_model( model[3] );
	}

	// normalise samples and values
	int i, j;
	mean[0] = 0; mean[1] = 0; mean[2] = 0; mean[3] = 0; mean[4] = 0; mean[5] = 0; mean[6] = 0;
	stdv[0] = 0; stdv[1] = 0; stdv[2] = 0; stdv[3] = 0; stdv[4] = 0; stdv[5] = 0; stdv[6] = 0;
	for ( i=0; i<numOfSamples; ++i ) {
		mean[0] += sample[i][0].value;
		mean[1] += sample[i][1].value;
		mean[2] += sample[i][2].value;
		mean[3] += prob[0].y[i];
		mean[4] += prob[1].y[i];
		mean[5] += prob[2].y[i];
		mean[6] += prob[3].y[i];
	}
	mean[0] /= numOfSamples;
	mean[1] /= numOfSamples;
	mean[2] /= numOfSamples;
	mean[3] /= numOfSamples;
	mean[4] /= numOfSamples;
	mean[5] /= numOfSamples;
	mean[6] /= numOfSamples;

	for ( i=0; i<numOfSamples; ++i ) {
		stdv[0] += (sample[i][0].value - mean[0])*(sample[i][0].value - mean[0]);
		stdv[1] += (sample[i][1].value - mean[1])*(sample[i][1].value - mean[1]);
		stdv[2] += (sample[i][2].value - mean[2])*(sample[i][2].value - mean[2]);
		stdv[3] += (prob[0].y[i] - mean[3])*(prob[0].y[i] - mean[3]);
		stdv[4] += (prob[1].y[i] - mean[4])*(prob[1].y[i] - mean[4]);
		stdv[5] += (prob[2].y[i] - mean[5])*(prob[2].y[i] - mean[5]);
		stdv[6] += (prob[3].y[i] - mean[6])*(prob[3].y[i] - mean[6]);
	}
	stdv[0] = sqrt(stdv[0]/(numOfSamples-1));
	stdv[1] = sqrt(stdv[1]/(numOfSamples-1));
	stdv[2] = sqrt(stdv[2]/(numOfSamples-1));
	stdv[3] = sqrt(stdv[3]/(numOfSamples-1));
	stdv[4] = sqrt(stdv[4]/(numOfSamples-1));
	stdv[5] = sqrt(stdv[5]/(numOfSamples-1));
	stdv[6] = sqrt(stdv[6]/(numOfSamples-1));

	for ( i=0; i<numOfSamples; ++i ) {
		sample[i][0].value -= mean[0]; sample[i][0].value /= stdv[0];
		sample[i][1].value -= mean[1]; sample[i][1].value /= stdv[1];
		sample[i][2].value -= mean[2]; sample[i][2].value /= stdv[2];
		prob[0].y[i] -= mean[3]; prob[0].y[i] /= stdv[3];
		prob[1].y[i] -= mean[4]; prob[1].y[i] /= stdv[4];
		prob[2].y[i] -= mean[5]; prob[2].y[i] /= stdv[5];
		prob[3].y[i] -= mean[6]; prob[3].y[i] /= stdv[6];
	}

	model[0] = svm_train( &prob[0], &param );
	model[1] = svm_train( &prob[1], &param );
	model[2] = svm_train( &prob[2], &param );
	model[3] = svm_train( &prob[3], &param );

	FILE* out = fopen("x0_data.txt", "w");
	for ( i=0; i<numOfSamples; ++i ) {
		fprintf( out, "%lf ", prob[0].y[i]);
		for ( j=0; j<3; ++j ) {
			fprintf( out, "%d:%lf ", prob[0].x[i][j].index, prob[0].x[i][j].value);
		}
		fprintf( out, "\n");
	}
	fclose(out);
	svm_save_model( "x0_model.txt", model[0] );

}
	
void BodyMapLearningBlockStruct::predict( double x, double y, double z )
{

	// fill new sample whose value to predict
	newSample[0].index = 1; newSample[0].value = (x-mean[0])/stdv[0];
	newSample[1].index = 2; newSample[1].value = (y-mean[1])/stdv[1];
	newSample[2].index = 3; newSample[2].value = (z-mean[2])/stdv[2];
	newSample[3].index = -1; newSample[3].value = 0;

	// predict !!
	x0 = (int) (svm_predict(model[0], newSample)*stdv[3]+mean[3]);
	y0 = (int) (svm_predict(model[1], newSample)*stdv[4]+mean[4]);
	x1 = (int) (svm_predict(model[2], newSample)*stdv[5]+mean[5]);
	y1 = (int) (svm_predict(model[3], newSample)*stdv[6]+mean[6]);

}
