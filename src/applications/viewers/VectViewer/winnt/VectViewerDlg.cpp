// VectViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VectViewer.h"
#include "VectViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectViewerDlg dialog

CVectViewerDlg::CVectViewerDlg(const char *name, int period, int size, int window, CWnd* pParent /*=NULL*/)
	: CDialog(CVectViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVectViewerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	strncpy(_name, name, 255);

	_window = window;
	_size = size;
	_period = period;

	_scale = new double [_size];
	_max = new double [_size];
}

void CVectViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVectViewerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVectViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CVectViewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectViewerDlg message handlers

BOOL CVectViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// set timer
	_timer = SetTimer(1, _period, 0);

	// display stuff
	
	CClientDC dcScreen(this);
	_dspWindows = new CBitmap[_size];
	_dcMem = new CDC[_size];

	_history = new double *[_size];
	
	int i = 0;
	for (i = 0; i < _size; i++)
	{
		_dspWindows[i].CreateCompatibleBitmap(&dcScreen, _window, __ySize);
		_dcMem[i].CreateCompatibleDC(&dcScreen);
		_dcMem[i].SelectObject(&_dspWindows[i]);

		_history[i] = new double[_window];
		memset(_history[i], 0, sizeof(double)*_window);
	}

	_nRows = _size;
	_nColumns = 1;

	_displayRect.SetRect(__dispYPos, __dispXPos, (_window+__xDist)*_nColumns, (__ySize+__yDist)*_nRows);

	// resize window
	this->SetWindowPos(&wndTop, 0, 0, (_window+__xDist)*_nColumns, (__ySize+__yDist)*_nRows+34, SWP_HIDEWINDOW);
	
	// zoom factors
	_zoom = new double [_size];
	for (i = 0; i < _size; i++)
			_zoom[i] = 1; //0.001;

	// initialize display
	for(i = 0; i < _size; i++)
	{
		_current[i] = __ySize * 0.5;
		_previous[i] = __ySize * 0.5;
	}

	// receiver
	_pReceiver = new CRecv(this);
	_pReceiver->Register(_name);
	_pReceiver->Begin();

	_counter = 0;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVectViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVectViewerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	int xPos = __dispXPos;
	int yPos = __dispYPos;

	// plot bitmap(s)
	int c = 1;
	int r = 1;

	for (int i = 0; i < _size; i++)
	{
		// compute display position
		xPos = (_window + __xDist)*(c-1);
		yPos = (__ySize + __yDist)*(r-1);

		dc.BitBlt(xPos, yPos, _window, __ySize, &_dcMem[i], 0, 0, SRCCOPY);

		if (i == _nRows-1)
		{
			// end of the column, increase 'c' reset 'r'
			r = 1;
			c++;
		}
		else
			r++;
	}
	
	CDialog::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVectViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVectViewerDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CVectViewerDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	_mutex.Wait();
	// scroll
	for (int i = 0; i < _size; i++)
	{	
		ScrollAndPaint(_dcMem[i], _current[i], _previous[i]);
		_previous[i] = _current[i];
	}
	_mutex.Post();

	if (IsWindowVisible())
		InvalidateRect(_displayRect, FALSE);
	
	CDialog::OnTimer(nIDEvent);
}

void CVectViewerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// destroy stuff
	_pReceiver->End();		// stop receiver thread
	KillTimer(_timer);		// stop timer

	delete [] _dcMem;
	delete [] _dspWindows;
	delete [] _zoom;
	delete _pReceiver;
	delete [] _scale;
	delete [] _max;

	for(int i = 0; i < _size; i++)
	{
		delete [] _history[i];
	}

	delete [] _history;
}

void CVectViewerDlg::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CDialog::OnCaptureChanged(pWnd);
}
