// VectViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VectViewer.h"
#include "VectViewerDlg.h"

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPString.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int __defaultPeriod = 100;
const int __defaultSize = 1;
const int __defaultWindowLength = 300;
const int __defaultWindowHeight = 100;


/////////////////////////////////////////////////////////////////////////////
// CVectViewerApp

BEGIN_MESSAGE_MAP(CVectViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CVectViewerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectViewerApp construction

CVectViewerApp::CVectViewerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVectViewerApp object

CVectViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVectViewerApp initialization

BOOL CVectViewerApp::InitInstance()
{
	// parse command line

	int iPeriod;
	int iSize;
	int iLength;
	int iHeight;
	double *dScale;
	bool aScale;
	YARPString name, network;
	
	if (!YARPParseParameters::parse(__argc, __argv, "-name", name))
		name = "/defaultVectorViewer";

	if (!YARPParseParameters::parse(__argc, __argv, "-net", network))
		network = "default";

	if (!YARPParseParameters::parse(__argc, __argv, "-p", &iPeriod))
		iPeriod = __defaultPeriod;

	if (!YARPParseParameters::parse(__argc, __argv, "-size", &iSize))
		iSize = __defaultSize;

	dScale = new double[iSize];

	if (!YARPParseParameters::parse(__argc, __argv, "-length", &iLength))
		iLength = __defaultWindowLength;

	if (!YARPParseParameters::parse(__argc, __argv, "-height", &iHeight))
		iHeight = __defaultWindowHeight;

	double sc;
	double dTmp;
	if (YARPParseParameters::parse(__argc, __argv, "-scale", &dTmp))
	{
		sc = 1/dTmp;
		aScale = false;
	}
	else
	{
		sc = 1.0;
		aScale = true;
	}
		
	for(int i = 0; i < iSize; i++)
		dScale[i] = sc;
	

	// window
	///////////////////////////////////////////
	if (!YARPParseParameters::parse(__argc, __argv, "-x", &_posX))
		_posX = -1;

	if (!YARPParseParameters::parse(__argc, __argv, "-y", &_posY))
		_posY = -1;

	if (!YARPParseParameters::parse(__argc, __argv, "-w", &_width))
		_width = -1;

	if (!YARPParseParameters::parse(__argc, __argv, "-h", &_height))
		_height = -1;
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CVectViewerDlg dlg(name.c_str(), network.c_str(), iPeriod, iSize, iLength, iHeight);
	m_pMainWnd = &dlg;
	
	dlg._aScale = aScale;
	dlg.setScale(dScale);
	
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
