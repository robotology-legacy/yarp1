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

CVectViewerDlg::CVectViewerDlg(const char *name, const char *net, int period, int size, int xS, int yS, CWnd* pParent /*=NULL*/)
	: CDialog(CVectViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVectViewerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	strncpy(_name, name, 256);
	strncpy(_network, net, 256);

	_xSize = xS;
	_ySize = yS;
	_size = size;
	_period = period;

	_scale = new double [_size];
	_max = new double [_size];
}

CVectViewerDlg::~CVectViewerDlg()
{
	delete[] _scale;
	delete[] _max;
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
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_IMAGE_FREEZE, OnImageFreeze)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_FREEZE, OnUpdateImageFreeze)
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

	// fottuto menu
	_menu.LoadMenu(IDR_MENUMAIN);
	SetMenu(&_menu);
	/////////////////

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
		_dspWindows[i].CreateCompatibleBitmap(&dcScreen, _xSize, _ySize);
		_dcMem[i].CreateCompatibleDC(&dcScreen);
		_dcMem[i].SelectObject(&_dspWindows[i]);

		_history[i] = new double[_xSize];
		memset(_history[i], 0, sizeof(double)*_xSize);
	}

	SetWindowText(_name);

	_nRows = _size;
	_nColumns = 1;
		
	_displayRect.SetRect(__dispYPos, __dispXPos, __dispXPos+(_xSize+__xDist)*_nColumns, __dispYPos+(_ySize+__yDist)*_nRows);

	int MENU_HEIGHT = 54; // LATER: GET MENU HEIGHT
	// resize window
	this->SetWindowPos(&wndTop, 0, 0, __dispXPos+(_xSize+__xDist)*_nColumns, __dispYPos+(_ySize+__yDist)*_nRows + MENU_HEIGHT, SWP_HIDEWINDOW);
	
	// zoom factors
	_zoom = new double [_size];
	for (i = 0; i < _size; i++)
			_zoom[i] = 1; //0.001;

	_current = new double [_size];
	_previous = new double [_size];
	// initialize display
	for(i = 0; i < _size; i++)
	{
		_current[i] = _ySize * 0.5;
		_previous[i] = _ySize * 0.5;
	}

	// receiver
	_pReceiver = new CRecv(this);
	_pReceiver->Register(_name, _network);
	_pReceiver->Begin();

	_counter = 0;

	///// window size and pos
	CVectViewerApp *p = ((CVectViewerApp *)AfxGetApp());

	CRect wpos;
	GetWindowRect (&wpos);
	ClientToScreen (&wpos);

	if (p->_posX < 0) p->_posX = wpos.left;
	if (p->_posY < 0) p->_posY = wpos.top;
	if (p->_width < 0) p->_width = wpos.Width();
	if (p->_height < 0) p->_height = wpos.Height();

	MoveWindow (p->_posX, p->_posY, p->_width, p->_height, TRUE);
	//////////////

	// 
	_freeze = FALSE;

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
		xPos = __dispXPos+(_xSize + __xDist)*(c-1);
		yPos = __dispYPos+(_ySize + __yDist)*(r-1);

		dc.BitBlt(xPos, yPos, _xSize, _ySize, &_dcMem[i], 0, 0, SRCCOPY);

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

	if (IsWindowVisible())
		InvalidateRect(_displayRect, FALSE);
	
	CDialog::OnTimer(nIDEvent);
}

void CVectViewerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// destroy stuff
	_pReceiver->End(2000);		// stop receiver thread
	KillTimer(_timer);		// stop timer

	delete [] _dcMem;
	delete [] _dspWindows;
	delete [] _zoom;
	delete [] _previous;
	delete [] _current;
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

void CVectViewerDlg::OnFileSave() 
{
	_freeze = TRUE;
	CFileDialog fileDlg(FALSE,
						"txt",
						NULL,
						OFN_OVERWRITEPROMPT,
						NULL,
						this);

	int nResponse = fileDlg.DoModal();
	if (nResponse == IDOK)
	{
		SaveCurrentFrame (fileDlg.m_ofn.lpstrFile);
	}

	_freeze = FALSE;
}

void CVectViewerDlg::SaveCurrentFrame(const char *filename)
{
	// save frames
	FILE *fp;
	fp = fopen(filename,"w");

	int i = 0;
	int v = 0;
	for(i = 0; i<_xSize; i++)
	{
		for(v = 0; v<_size; v++)
		{
			fprintf(fp, "%lf\t", _history[v][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void CVectViewerDlg::OnImageFreeze() 
{
	_freeze = !_freeze;
}

void CVectViewerDlg::OnUpdateImageFreeze(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}
void CVectViewerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if (!bSysMenu)
	{
		ASSERT(pPopupMenu != NULL);
		// check the enabled state of various menu items
		CCmdUI state;
		state.m_pMenu = pPopupMenu;
		ASSERT(state.m_pOther == NULL);
		state.m_nIndexMax = pPopupMenu->GetMenuItemCount();

		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
		{
			state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
			if (state.m_nID == 0)
				continue; // menu separator or invalid cmd - ignore it
			ASSERT(state.m_pOther == NULL);
			ASSERT(state.m_pMenu != NULL);
			if (state.m_nID == (UINT)-1)
			{
				// possibly a popup menu, route to first item of that popup
				state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
				if (state.m_pSubMenu == NULL || 
					(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
					state.m_nID == (UINT)-1)
				{
					continue; // first item of popup can't be routed to
				}
				state.DoUpdate(this, FALSE);  // popups are never auto disabled
			}
			else
			{
				// normal menu item
				// Auto enable/disable if command is _not_ a system command
				state.m_pSubMenu = NULL;
				state.DoUpdate(this, state.m_nID < 0xF000);
			}
		}
	}	
}
