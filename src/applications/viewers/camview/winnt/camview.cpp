// camview.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "camview.h"
#include "camviewDlg.h"

#include "CommandLineInfoEx.h"

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
	CCommandLineInfoEx cmdInfo; 
	ParseCommandLine (cmdInfo);

	m_portname = "/view/i:img";
	if (cmdInfo.GetOption("name", m_portname)) 
	{ 
		/// adds the leading /
		m_portname = "/" + m_portname;
	}
	
	m_out_portname = "/view/o:point";
	if (cmdInfo.GetOption("out", m_out_portname )) 
	{ 
		/// adds the leading /
		m_out_portname = "/" + m_out_portname ;
		m_enable_output = true;
	}
	else
	{
		/// default value if not specified, outport is actually disabled.
		m_out_portname = m_portname + "/out";
		m_enable_output = false;
	}

	m_netname = "default";
	cmdInfo.GetOption("net", m_netname);

	CString speriod;
	if (cmdInfo.GetOption("p", speriod))
	{
		m_period = atoi(speriod);
	}
	else
		m_period = 0;

	CString tmp;
	if (cmdInfo.GetOption("x", tmp))
		m_x = atoi(tmp);
	else
		m_x = -1;

	if (cmdInfo.GetOption("y", tmp))
		m_y = atoi(tmp);
	else
		m_y = -1;

	if (cmdInfo.GetOption("w", tmp))
		m_width = atoi(tmp);
	else
		m_width = -1;

	if (cmdInfo.GetOption("h", tmp))
		m_height = atoi(tmp);
	else
		m_height = -1;
	
	m_lp = (cmdInfo.GetOption("l")) ? true : false;
	m_fov = (cmdInfo.GetOption("f")) ? true : false;

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
