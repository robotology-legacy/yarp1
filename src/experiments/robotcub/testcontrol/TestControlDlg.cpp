// TestControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "TestControlDlg.h"

#include <yarp/YARPScheduler.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
// robot interface instances. 
//
YARPRobotcubHead head;


//
// helper functions.
// 
static HANDLE	__console_handle = INVALID_HANDLE_VALUE;

static BOOL init_console (void)
{
	if (__console_handle == INVALID_HANDLE_VALUE)
	{
		BOOL ret = AllocConsole ();	
		SetConsoleTitle ("Can bus spy window");
		__console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		return ret;
	}
	else
		return FALSE;
}

static BOOL close_console (void)
{
	CloseHandle(__console_handle);
	__console_handle = INVALID_HANDLE_VALUE;
	return FreeConsole ();
}

/// Use wprintf like TRACE0, TRACE1, ... (The arguments are the same as printf)
static int xprintf(char *fmt, ...)
{
	if (__console_handle == INVALID_HANDLE_VALUE)
		return -1;

	char s[300];
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);

	DWORD cCharsWritten;
	WriteConsole(__console_handle, s, ACE_OS::strlen(s), &cCharsWritten, NULL);

	return(cnt);
}


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
// CTestControlDlg dialog

CTestControlDlg::CTestControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_FACE);

	_headinitialized = false;
}

void CTestControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestControlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestControlDlg, CDialog)
	//{{AFX_MSG_MAP(CTestControlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_WM_CLOSE()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_INTERFACE_START, OnInterfaceStart)
	ON_COMMAND(ID_INTERFACE_STOP, OnInterfaceStop)
	ON_UPDATE_COMMAND_UI(ID_INTERFACE_START, OnUpdateInterfaceStart)
	ON_UPDATE_COMMAND_UI(ID_INTERFACE_STOP, OnUpdateInterfaceStop)
	ON_COMMAND(ID_FILE_OPENCONSOLE, OnFileOpenconsole)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENCONSOLE, OnUpdateFileOpenconsole)
	ON_COMMAND(ID_FILE_CLOSECONSOLE, OnFileCloseconsole)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSECONSOLE, OnUpdateFileCloseconsole)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestControlDlg message handlers

BOOL CTestControlDlg::OnInitDialog()
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
	
	/// specific initialization.
	YARPScheduler::setHighResScheduling();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestControlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CTestControlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestControlDlg::OnHelpAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CTestControlDlg::OnFileExit() 
{
	PostMessage (WM_CLOSE);	
}

void CTestControlDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

void CTestControlDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
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

void CTestControlDlg::EnableGUI ()
{
	SetTimer (TIMER_ID, GUI_REFRESH_INTERVAL, NULL); 
}

void CTestControlDlg::DisableGUI ()
{
	KillTimer (TIMER_ID);
}

void CTestControlDlg::OnInterfaceStart() 
{
	char *root = GetYarpRoot();
	char path[512];

	ACE_OS::sprintf (path, "%s/%s\0", root, ConfigFilePath); 

	int ret = head.initialize(YARPString(path), YARPString(INI_FILE));
	if (ret != YARP_OK)
	{
		_headinitialized = false;
		DisableGUI ();
		char message[512];
		sprintf (message, "Can't start the interface with the robot, please make sure the hardware and control cards are on.\nThe ini file was: %s%s", path, INI_FILE);
		MessageBox (message, "Error!");
		return;
	}

	_headinitialized = true;

	EnableGUI ();
}

void CTestControlDlg::OnInterfaceStop() 
{
	close_console();

	head.idleMode ();
	head.uninitialize ();	
	
	DisableGUI ();
	_headinitialized = false;
}

void CTestControlDlg::OnUpdateInterfaceStart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!_headinitialized);
}

void CTestControlDlg::OnUpdateInterfaceStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized);
}

void CTestControlDlg::OnFileOpenconsole() 
{
	init_console();
}

void CTestControlDlg::OnUpdateFileOpenconsole(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized);
}

void CTestControlDlg::OnFileCloseconsole() 
{
	close_console();
}

void CTestControlDlg::OnUpdateFileCloseconsole(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized);
}
