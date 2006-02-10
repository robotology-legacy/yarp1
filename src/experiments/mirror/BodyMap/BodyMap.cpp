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

BodyMapLearningBlockStruct::BodyMapLearningBlockStruct( CWnd* dlg ) : _myDlg(dlg)
{

	// learning block constructor

	// initialise basic data types
	x = y = z = x0 = y0 = x1 = y1 = 0;
	sampleCount = 0;

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

	// initialise problem. sample buffer is fixed, and so
	// is the dimension of the samples.
	prob.l = numOfSamples;
	prob.y = (double*) calloc( 1, sizeof(double)*prob.l );
	prob.x = (struct svm_node**) calloc( 1, sizeof(struct svm_node*)*prob.l );
	for ( int i=0; i<prob.l; ++i ) {
		prob.x[i] = (struct svm_node*) malloc( sizeof(struct svm_node)*4 );
	}

	// check paramters. problem is not required unless we use NU-SVC,
	// which is never the case for us.
	const char* error_msg;
	error_msg = svm_check_parameter( &prob, &param );
	if ( error_msg != NULL ) {
		exit(YARP_FAIL);
	}

	train();

	return;

}

BodyMapLearningBlockStruct::~BodyMapLearningBlockStruct()
{

	// learning block destructor
    svm_destroy_param( &param );
    free( prob.y );
	free( prob.x );

}

void BodyMapLearningBlockStruct::predict()
{

	// fill new sample whose value to predict
	newSample[0].index = 1; newSample[0].value = x;
	newSample[1].index = 2; newSample[1].value = y;
	newSample[2].index = 3; newSample[2].value = z;
	newSample[3].index = -1; newSample[3].value = 0;

	// predict !!
	x0 = svm_predict( model, newSample );

}

int BodyMapLearningBlockStruct::addSample(double x, double y, double z, double value)
{

	// add a new sample to our training set.

	// if the buffer is full, stop and return failure, but reset
	// counter, so that next time it will be ok
	if ( sampleCount == numOfSamples ) {
		sampleCount = 0;
		return YARP_FAIL;
	} else {
		// otherwise, store a new sample and go on
		prob.x[sampleCount][0].index = 1; prob.x[sampleCount][0].value = x;
		prob.x[sampleCount][1].index = 2; prob.x[sampleCount][1].value = y;
		prob.x[sampleCount][2].index = 3; prob.x[sampleCount][2].value = z;
		prob.x[sampleCount][3].index = -1; prob.x[sampleCount][3].value = 0;
		prob.y[sampleCount] = value;

char title[50];
ACE_OS::sprintf(title, "%d samples loaded...", sampleCount+1);
AfxGetMainWnd()->SetWindowText(title);

		sampleCount++;
		return YARP_OK;
	}

}

void BodyMapLearningBlockStruct::train()
{

	model = svm_train( &prob, &param );

}
	
void FindTrackerXY(YARPImageOf<YarpPixelBGR>& img, int* x, int* y, int* w)
{

	unsigned long sumX = 0, sumY = 0, nOfPixels = 0;

	IMGFOR(img,i,j) {
		
		double r = img(i,j).r/255.0;
		double g = img(i,j).g/255.0;
		double b = img(i,j).b/255.0;
		double h, s, v;
		double max = (r>=b && r>=g) ? r : ((g>=r && g>=b) ? g : b );
		double min = (r<=b && r<=g) ? r : ((g<=r && g<=b) ? g : b );

		if ( max == min ) {
			h = 0;
		} else {
			if ( max == r ) h = 60*(g-b)/(max-min)+0;
			if ( max == g ) h = 60*(b-r)/(max-min)+120;
			if ( max == b ) h = 60*(r-g)/(max-min)+240;
		}
		if ( max == 0 ) {
			s = 0;
		} else {
			s = (max-min)/max;
		}
		v = max;

		if ( cos(h/360*2*M_PI)>0.85 && s>0.5 && v>0.65 ) {
			sumX += i;
			sumY += j;
			++nOfPixels;
		}
	}

	if ( nOfPixels > 0 ) {
		*x = (int)(sumX/nOfPixels);
		*y = (int)(sumY/nOfPixels);
		*w = nOfPixels;
	} else {
		*x = -1;
		*y = -1;
		*w = 0;
	}

}
