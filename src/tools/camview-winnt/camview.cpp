// camview.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "camview.h"
#include "camviewDlg.h"

#include <yarp/YARPParseParameters.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCamviewApp

BEGIN_MESSAGE_MAP(CCamviewApp, CWinApp)
	//{{AFX_MSG_MAP(CCamviewApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCamviewApp construction

CCamviewApp::CCamviewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCamviewApp object

CCamviewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCamviewApp initialization

BOOL CCamviewApp::InitInstance()
{
	int argc = __argc;
	char **argv = __argv;

	m_portname = "/view/i:img";
	char dummy[255];
	if(YARPParseParameters::parse(argc, argv, "-name", dummy))
	{ 
		m_portname = dummy;
	}
	
	m_out_portname = "/view/o:point";
	if (YARPParseParameters::parse(argc, argv, "-out", dummy)) 
	{ 
		/// adds the leading /
		m_out_portname = dummy;
		m_enable_output = true;
	}
	else
	{
		/// default value if not specified, outport is actually disabled.
		m_out_portname = m_portname + "/out";
		m_enable_output = false;
	}

	if (YARPParseParameters::parse(argc, argv, "-net", dummy))
		m_netname = dummy;
	else
		m_netname = "default";

	if (YARPParseParameters::parse(argc, argv, "-neto", dummy))
		m_out_netname = dummy;
	else
		m_out_netname = "default";

	int itmp;
	if (YARPParseParameters::parse(argc, argv, "-p", &itmp))
		m_period = itmp;
	else
		m_period = 0;

	if (YARPParseParameters::parse(argc, argv, "-x", &itmp))
		m_x = itmp;
	else
		m_x = -1;

	if (YARPParseParameters::parse(argc, argv, "-y", &itmp))
		m_y = itmp;
	else
		m_y = -1;

	if (YARPParseParameters::parse(argc, argv, "-w", &itmp))
		m_width = itmp;
	else
		m_width = -1;

	if (YARPParseParameters::parse(argc, argv, "-h", &itmp))
		m_height = itmp;
	else
		m_height = -1;

	if (YARPParseParameters::parse(argc, argv, "-l"))
		m_lp = true;
	else 
		m_lp = false;
	
	if (YARPParseParameters::parse(argc, argv, "-f"))
		m_fov = true;
	else 
		m_fov = false;

	if (YARPParseParameters::parse(argc, argv, "-flip"))
		m_horiz_flip = true;
	else 
		m_horiz_flip = false;

	/// not really needed.
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CCamviewDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	/// is this needed?
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
