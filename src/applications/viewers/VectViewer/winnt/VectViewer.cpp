// VectViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VectViewer.h"
#include "VectViewerDlg.h"

#include "CommandLineInfoEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int __defaultPeriod = 100;
const int __defaultSize = 1;
const int __defaultWindowLength = 300;

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

	CCommandLineInfoEx cmdInfo;
	ParseCommandLine(cmdInfo);

	int iPeriod;
	int iSize;
	int iWindow;
	double *dScale;
	bool aScale;
	CString dummy,name, period, size, window, scale, x, y, w, h;
	name = "/";
	cmdInfo.GetOption("name", dummy);
	name += dummy;
	if (cmdInfo.GetOption("p", period))
		iPeriod = atoi(period);
	else
		iPeriod = __defaultPeriod;

	if (cmdInfo.GetOption("size", size))
		iSize = atoi(size);
	else
		iSize = __defaultSize;

	dScale = new double[iSize];

	if (cmdInfo.GetOption("window", window))
		iWindow = atoi(window);
	else
		iWindow = __defaultWindowLength;

	// if (cmdInfo.GetOption("scale", scale))
	for(int i = 0; i < iSize; i++)
		dScale[i] = 1.0;
	aScale = true;

	if (cmdInfo.GetOption("x", x))
		_posX = atoi(x);
	else
		_posX = -1;

	if (cmdInfo.GetOption("y", y))
		_posY = atoi(y);
	else
		_posY = -1;

	if (cmdInfo.GetOption("w", w))
		_width = atoi(w);
	else
		_width = -1;

	if (cmdInfo.GetOption("h", h))
		_height = atoi(h);
	else
		_height = -1;

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CVectViewerDlg dlg(name, iPeriod, iSize, iWindow);
	m_pMainWnd = &dlg;

	dlg._aScale = aScale;
	dlg.setScale(dScale);

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
