// CanControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CanControl.h"
#include "CanControlDlg.h"
#include "CanControlParams.h"

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
// CCanControlDlg dialog

CCanControlDlg::CCanControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCanControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCanControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCanControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCanControlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCanControlDlg, CDialog)
	//{{AFX_MSG_MAP(CCanControlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_DRIVER_RUN, OnDriverRun)
	ON_COMMAND(ID_DRIVER_KILL, OnDriverKill)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCanControlDlg message handlers

BOOL CCanControlDlg::OnInitDialog()
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
	
	m_njoints = DEFAULT_NJOINTS;
	memset (m_destinations, 16, sizeof(unsigned char) * CANBUS_MAXCARDS);
	m_destinations[0] = DEFAULT_DESTINATION;
	m_destinations[15] = CANBUS_MY_ADDRESS;		/// shouldn't be used anyway
	m_driverok = false;


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCanControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCanControlDlg::OnPaint() 
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
//  the minimized window.
HCURSOR CCanControlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCanControlDlg::OnHelpAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CCanControlDlg::OnClose() 
{
	OnDriverKill ();
	CDialog::OnClose();
}

void CCanControlDlg::OnFileExit() 
{
	PostMessage (WM_CLOSE);	
}

void CCanControlDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
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

///
/// starts up the device driver.
///
void CCanControlDlg::OnDriverRun() 
{
	/// we have a few defaults here.
	m_params._port_number = CANBUS_DEVICE_NUM;
	m_params._arbitrationID = CANBUS_ARBITRATION_ID;
	
	memcpy (m_params._destinations, m_destinations, sizeof(unsigned char) * CANBUS_MAXCARDS);

	m_params._my_address = CANBUS_MY_ADDRESS;					/// my address.
	m_params._polling_interval = CANBUS_POLLING_INTERVAL;		/// thread polling interval [ms].
	m_params._timeout = CANBUS_TIMEOUT;							/// approx this value times the polling interval [ms].

	m_params._njoints = m_njoints;

	if (m_driver.open ((void *)&m_params) < 0)
	{
		m_driverok = false;
		m_driver.close ();
		MessageBox ("The driver didn't start correctly, pls check the hardware", "Error!");
		return;
	}

	MessageBox ("The driver is now running", "Good!");
	m_driverok = true;
}

void CCanControlDlg::OnDriverKill() 
{
	if (m_driverok)
	{
		m_driver.close ();
		m_driverok = false;
	}
}
