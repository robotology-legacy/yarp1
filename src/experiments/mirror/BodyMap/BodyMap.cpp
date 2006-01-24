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


void FindTrackerXY(YARPImageOf<YarpPixelBGR>& img, int* x, int* y)
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

		if ( cos(h/360*2*M_PI)>0.85 && s>0.5 && v>0.85 ) {
			sumX += i;
			sumY += j;
			++nOfPixels;
		}
	}

	if ( nOfPixels > 0 ) {
		*x = (int)(sumX/nOfPixels);
		*y = (int)(sumY/nOfPixels);
	} else {
		*x = -1;
		*y = -1;
	}

}
