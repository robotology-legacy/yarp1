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
YARPHead head;
YARPArm arm;


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
//		HWND wnd = GetConsoleWindow ();
//		ShowWindow (wnd, SW_SHOW);
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
	m_p1 = 0.0;
	m_p2 = 0.0;
	m_p3 = 0.0;
	m_p4 = 0.0;
	m_p5 = 0.0;
	m_p6 = 0.0;
	m_p7 = 0.0;
	m_p8 = 0.0;
	m_v1 = 0.0;
	m_v2 = 0.0;
	m_v3 = 0.0;
	m_v4 = 0.0;
	m_v5 = 0.0;
	m_v6 = 0.0;
	m_v7 = 0.0;
	m_v8 = 0.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_FACE);

	_headinitialized = false;
	_headjoints = -1;
	_headjointstore = NULL;
	_headlastreached = NULL;

	int i;
	for (i = 0; i < N_POSTURES; i++)
	{
		_headstore[i] = NULL;
		_headstorev[i] = NULL;
	}
}

void CTestControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestControlDlg)
	DDX_Control(pDX, IDC_BUTTON_0ENCODERS, m_0encoders_ctrl);
	DDX_Control(pDX, IDC_BUTTON_CALIBRATEHEAD, m_calibratehead_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STORE_CURRENT, m_storecurrent_ctrl);
	DDX_Control(pDX, IDC_EDIT_S8, m_s8_ctrl);
	DDX_Control(pDX, IDC_EDIT_S7, m_s7_ctrl);
	DDX_Control(pDX, IDC_EDIT_S6, m_s6_ctrl);
	DDX_Control(pDX, IDC_EDIT_S5, m_s5_ctrl);
	DDX_Control(pDX, IDC_EDIT_S4, m_s4_ctrl);
	DDX_Control(pDX, IDC_EDIT_S3, m_s3_ctrl);
	DDX_Control(pDX, IDC_EDIT_S2, m_s2_ctrl);
	DDX_Control(pDX, IDC_EDIT_S1, m_s1_ctrl);
	DDX_Control(pDX, IDC_EDIT_V8, m_v8_ctrl);
	DDX_Control(pDX, IDC_EDIT_V7, m_v7_ctrl);
	DDX_Control(pDX, IDC_EDIT_V6, m_v6_ctrl);
	DDX_Control(pDX, IDC_EDIT_V5, m_v5_ctrl);
	DDX_Control(pDX, IDC_EDIT_V4, m_v4_ctrl);
	DDX_Control(pDX, IDC_EDIT_V3, m_v3_ctrl);
	DDX_Control(pDX, IDC_EDIT_V2, m_v2_ctrl);
	DDX_Control(pDX, IDC_EDIT_V1, m_v1_ctrl);
	DDX_Control(pDX, IDC_EDIT_P8, m_p8_ctrl);
	DDX_Control(pDX, IDC_EDIT_P7, m_p7_ctrl);
	DDX_Control(pDX, IDC_EDIT_P6, m_p6_ctrl);
	DDX_Control(pDX, IDC_EDIT_P5, m_p5_ctrl);
	DDX_Control(pDX, IDC_EDIT_P4, m_p4_ctrl);
	DDX_Control(pDX, IDC_EDIT_P3, m_p3_ctrl);
	DDX_Control(pDX, IDC_EDIT_P2, m_p2_ctrl);
	DDX_Control(pDX, IDC_EDIT_P1, m_p1_ctrl);
	DDX_Control(pDX, IDC_BUTTON_GO, m_go_ctrl);
	DDX_Control(pDX, IDC_COMBO_ENTRY, m_entry_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STORE, m_store_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_stop_ctrl);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_run_ctrl);
	DDX_Control(pDX, IDC_BUTTON_IDLE, m_idle_ctrl);
	DDX_Text(pDX, IDC_EDIT_P1, m_p1);
	DDX_Text(pDX, IDC_EDIT_P2, m_p2);
	DDX_Text(pDX, IDC_EDIT_P3, m_p3);
	DDX_Text(pDX, IDC_EDIT_P4, m_p4);
	DDX_Text(pDX, IDC_EDIT_P5, m_p5);
	DDX_Text(pDX, IDC_EDIT_P6, m_p6);
	DDX_Text(pDX, IDC_EDIT_P7, m_p7);
	DDX_Text(pDX, IDC_EDIT_P8, m_p8);
	DDX_Text(pDX, IDC_EDIT_V1, m_v1);
	DDV_MinMaxDouble(pDX, m_v1, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_V2, m_v2);
	DDV_MinMaxDouble(pDX, m_v2, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_V3, m_v3);
	DDV_MinMaxDouble(pDX, m_v3, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_V4, m_v4);
	DDV_MinMaxDouble(pDX, m_v4, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_V5, m_v5);
	DDV_MinMaxDouble(pDX, m_v5, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_V6, m_v6);
	DDV_MinMaxDouble(pDX, m_v6, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_V7, m_v7);
	DDV_MinMaxDouble(pDX, m_v7, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_V8, m_v8);
	DDV_MinMaxDouble(pDX, m_v8, -32768., 32767.);
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
	ON_COMMAND(ID_INTERFACE_SHOWGAIN, OnInterfaceShowgain)
	ON_COMMAND(ID_INTERFACE_HIDEGAIN, OnInterfaceHidegain)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_IDLE, OnButtonIdle)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_STORE, OnButtonStore)
	ON_CBN_SELENDOK(IDC_COMBO_ENTRY, OnSelendokComboEntry)
	ON_BN_CLICKED(IDC_BUTTON_STORE_CURRENT, OnButtonStoreCurrent)
	ON_COMMAND(ID_FILE_LOADPOSTURES, OnFileLoadpostures)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOADPOSTURES, OnUpdateFileLoadpostures)
	ON_COMMAND(ID_FILE_SAVEPOSTURES, OnFileSavepostures)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEPOSTURES, OnUpdateFileSavepostures)
	ON_BN_CLICKED(IDC_BUTTON_CALIBRATEHEAD, OnButtonCalibratehead)
	ON_BN_CLICKED(IDC_BUTTON_0ENCODERS, OnButton0encoders)
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

	// modeless dialog boxes.
	_gaincontroldlg.Create (CGainControlDlg::IDD, this);
	_gaincontroldlg.m_parent = this;

	_calibrationdlg.Create (CCalibrationDlg::IDD, this);

	// sort of initialization.
	m_entry_ctrl.SetCurSel(0);
	m_p1_ctrl.SetWindowText ("0");
	m_p2_ctrl.SetWindowText ("0");
	m_p3_ctrl.SetWindowText ("0");
	m_p4_ctrl.SetWindowText ("0");
	m_p5_ctrl.SetWindowText ("0");
	m_p6_ctrl.SetWindowText ("0");
	m_p7_ctrl.SetWindowText ("0");
	m_p8_ctrl.SetWindowText ("0");
	m_v1_ctrl.SetWindowText ("0");
	m_v2_ctrl.SetWindowText ("0");
	m_v3_ctrl.SetWindowText ("0");
	m_v4_ctrl.SetWindowText ("0");
	m_v5_ctrl.SetWindowText ("0");
	m_v6_ctrl.SetWindowText ("0");
	m_v7_ctrl.SetWindowText ("0");
	m_v8_ctrl.SetWindowText ("0");

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
	OnInterfaceStop ();
	
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

	_gaincontroldlg.EnableInterface();
	_calibrationdlg.EnableInterface();

	m_entry_ctrl.EnableWindow();
	m_go_ctrl.EnableWindow();
	m_idle_ctrl.EnableWindow();
	m_run_ctrl.EnableWindow();
	m_stop_ctrl.EnableWindow();
	m_store_ctrl.EnableWindow();
	m_storecurrent_ctrl.EnableWindow();
	m_p1_ctrl.EnableWindow();
	m_p2_ctrl.EnableWindow();
	m_p3_ctrl.EnableWindow();
	m_p4_ctrl.EnableWindow();
	m_p5_ctrl.EnableWindow();
	m_p6_ctrl.EnableWindow();
	m_p7_ctrl.EnableWindow();
	m_p8_ctrl.EnableWindow();
	m_v1_ctrl.EnableWindow();
	m_v2_ctrl.EnableWindow();
	m_v3_ctrl.EnableWindow();
	m_v4_ctrl.EnableWindow();
	m_v5_ctrl.EnableWindow();
	m_v6_ctrl.EnableWindow();
	m_v7_ctrl.EnableWindow();
	m_v8_ctrl.EnableWindow();
	m_calibratehead_ctrl.EnableWindow();
	m_0encoders_ctrl.EnableWindow();
}

void CTestControlDlg::DisableGUI ()
{
	m_entry_ctrl.EnableWindow(FALSE);
	m_go_ctrl.EnableWindow(FALSE);
	m_idle_ctrl.EnableWindow(FALSE);
	m_run_ctrl.EnableWindow(FALSE);
	m_stop_ctrl.EnableWindow(FALSE);
	m_store_ctrl.EnableWindow(FALSE);
	m_storecurrent_ctrl.EnableWindow(FALSE);
	m_p1_ctrl.EnableWindow(FALSE);
	m_p2_ctrl.EnableWindow(FALSE);
	m_p3_ctrl.EnableWindow(FALSE);
	m_p4_ctrl.EnableWindow(FALSE);
	m_p5_ctrl.EnableWindow(FALSE);
	m_p6_ctrl.EnableWindow(FALSE);
	m_p7_ctrl.EnableWindow(FALSE);
	m_p8_ctrl.EnableWindow(FALSE);
	m_v1_ctrl.EnableWindow(FALSE);
	m_v2_ctrl.EnableWindow(FALSE);
	m_v3_ctrl.EnableWindow(FALSE);
	m_v4_ctrl.EnableWindow(FALSE);
	m_v5_ctrl.EnableWindow(FALSE);
	m_v6_ctrl.EnableWindow(FALSE);
	m_v7_ctrl.EnableWindow(FALSE);
	m_v8_ctrl.EnableWindow(FALSE);
	m_calibratehead_ctrl.EnableWindow(FALSE);
	m_0encoders_ctrl.EnableWindow(FALSE);

	_gaincontroldlg.DisableInterface();
	_calibrationdlg.DisableInterface();

	KillTimer (TIMER_ID);
}

void CTestControlDlg::AllocArrays(int nj)
{
	_headjointstore = new double[nj];
	ACE_ASSERT (_headjointstore != NULL);
	ACE_OS::memset (_headjointstore, 0, sizeof(double) * nj);

	_headlastreached = new double[nj];
	ACE_ASSERT (_headlastreached != NULL);
	ACE_OS::memset (_headlastreached, 0, sizeof(double) * nj);

	int i;
	for (i = 0; i < N_POSTURES; i++)
	{
		_headstore[i] = new double[nj];
		_headstorev[i] = new double[nj];
		ACE_ASSERT (_headstore[i] != NULL && _headstorev[i]);
		ACE_OS::memset (_headstore[i], 0, sizeof(double) * nj);
		ACE_OS::memset (_headstorev[i], 0, sizeof(double) * nj);
	}
}

void CTestControlDlg::FreeArrays(void)
{
	if (_headjointstore != NULL) delete[] _headjointstore;
	_headjointstore = NULL;

	if (_headlastreached != NULL) delete[] _headlastreached;
	_headlastreached = NULL;

	int i;
	for (i = 0; i < N_POSTURES; i++)
	{
		if (_headstore[i] != NULL) delete[] _headstore[i];
		_headstore[i] = NULL;
		if (_headstorev[i] != NULL) delete[] _headstorev[i];
		_headstorev[i] = NULL;
	}
}

void CTestControlDlg::OnInterfaceStart() 
{
	char *root = GetYarpRoot();
	char path[512];

	ACE_OS::sprintf (path, "%s/%s\0", root, ConfigFilePath); 

	YARPRobotcubHeadParameters parameters;
	parameters.load (YARPString(path), YARPString(INI_FILE));
	parameters._message_filter = 20;
	parameters._p = xprintf;

	int ret = head.initialize(parameters);
	if (ret != YARP_OK)
	{
		_headinitialized = false;
		DisableGUI ();
		char message[512];
		ACE_OS::sprintf (message, "Can't start the interface with the robot, please make sure the hardware and control cards are on.\nThe ini file was: %s%s", path, INI_FILE);
		MessageBox (message, "Error!");
		return;
	}

	_headinitialized = true;
	_headjoints = head.nj();

	AllocArrays (_headjoints);

	EnableGUI ();
}

void CTestControlDlg::OnInterfaceStop() 
{
	close_console();

	if (_headinitialized)
	{
		head.idleMode ();
		head.uninitialize ();	
	}
	
	FreeArrays ();

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
	if (init_console())
		xprintf("Debug windows started...\n");
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

void CTestControlDlg::OnInterfaceShowgain() 
{
	_gaincontroldlg.ShowWindow(SW_SHOW);
}

void CTestControlDlg::OnInterfaceHidegain() 
{
	_gaincontroldlg.ShowWindow(SW_HIDE);
}

void CTestControlDlg::OnButtonRun() 
{
	head.activatePID (false);
}

void CTestControlDlg::OnButtonIdle() 
{
	head.idleMode ();
}

void CTestControlDlg::OnTimer(UINT nIDEvent) 
{
	int ret = head.getPositions (_headjointstore);
	ACE_OS::memcpy (_headlastreached, _headjointstore, sizeof(double) * _headjoints);

	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[0]);
	m_s1_ctrl.SetWindowText (_buffer);
	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[1]);
	m_s2_ctrl.SetWindowText (_buffer);
	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[2]);
	m_s3_ctrl.SetWindowText (_buffer);
	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[3]);
	m_s4_ctrl.SetWindowText (_buffer);
	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[4]);
	m_s5_ctrl.SetWindowText (_buffer);
	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[5]);
	m_s6_ctrl.SetWindowText (_buffer);
	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[6]);
	m_s7_ctrl.SetWindowText (_buffer);
	ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[7]);
	m_s8_ctrl.SetWindowText (_buffer);
	
	CDialog::OnTimer(nIDEvent);
}

void CTestControlDlg::OnButtonGo() 
{
	UpdateData (TRUE);

	_headjointstore[0] = m_v1;
	_headjointstore[1] = m_v2;
	_headjointstore[2] = m_v3;
	_headjointstore[3] = m_v4;
	_headjointstore[4] = m_v5;
	_headjointstore[5] = m_v6;
	_headjointstore[6] = m_v7;
	_headjointstore[7] = m_v8;

	head.setVelocities (_headjointstore);

	_headjointstore[0] = m_p1;
	_headjointstore[1] = m_p2;
	_headjointstore[2] = m_p3;
	_headjointstore[3] = m_p4;
	_headjointstore[4] = m_p5;
	_headjointstore[5] = m_p6;
	_headjointstore[6] = m_p7;
	_headjointstore[7] = m_p8;

	head.setPositions (_headjointstore);
}

void CTestControlDlg::OnButtonStore() 
{
	// saves the current displayed position into the store.
	UpdateData (TRUE);

	const int entry = m_entry_ctrl.GetCurSel();	

	if (entry == CB_ERR || entry < 0 || entry >= N_POSTURES)
	{
		MessageBox ("Invalid selection: don't know where to save data", "Error!");
		return;
	}

	_headstore[entry][0] = m_p1;
	_headstore[entry][1] = m_p2;
	_headstore[entry][2] = m_p3;
	_headstore[entry][3] = m_p4;
	_headstore[entry][4] = m_p5;
	_headstore[entry][5] = m_p6;
	_headstore[entry][6] = m_p7;
	_headstore[entry][7] = m_p8;

	_headstorev[entry][0] = m_v1;
	_headstorev[entry][1] = m_v2;
	_headstorev[entry][2] = m_v3;
	_headstorev[entry][3] = m_v4;
	_headstorev[entry][4] = m_v5;
	_headstorev[entry][5] = m_v6;
	_headstorev[entry][6] = m_v7;
	_headstorev[entry][7] = m_v8;
}

void CTestControlDlg::OnSelendokComboEntry() 
{
	const int entry = m_entry_ctrl.GetCurSel();	

	if (entry == CB_ERR || entry < 0 || entry >= N_POSTURES)
	{
		MessageBox ("Invalid selection", "Error!");
		return;
	}

	m_p1 = _headstore[entry][0];
	m_p2 = _headstore[entry][1];
	m_p3 = _headstore[entry][2];
	m_p4 = _headstore[entry][3];
	m_p5 = _headstore[entry][4];
	m_p6 = _headstore[entry][5];
	m_p7 = _headstore[entry][6];
	m_p8 = _headstore[entry][7];

	m_v1 = _headstorev[entry][0];
	m_v2 = _headstorev[entry][1];
	m_v3 = _headstorev[entry][2];
	m_v4 = _headstorev[entry][3];
	m_v5 = _headstorev[entry][4];
	m_v6 = _headstorev[entry][5];
	m_v7 = _headstorev[entry][6];
	m_v8 = _headstorev[entry][7];

	UpdateData(FALSE);
}

void CTestControlDlg::OnButtonStoreCurrent() 
{
	// saves the current displayed position into the store.
	UpdateData (TRUE);

	const int entry = m_entry_ctrl.GetCurSel();	

	if (entry == CB_ERR || entry < 0 || entry >= N_POSTURES)
	{
		MessageBox ("Invalid selection: don't know where to save data", "Error!");
		return;
	}

	ACE_OS::memcpy (_headstore[entry], _headlastreached, sizeof(double) * _headjoints);

	_headstorev[entry][0] = m_v1;
	_headstorev[entry][1] = m_v2;
	_headstorev[entry][2] = m_v3;
	_headstorev[entry][3] = m_v4;
	_headstorev[entry][4] = m_v5;
	_headstorev[entry][5] = m_v6;
	_headstorev[entry][6] = m_v7;
	_headstorev[entry][7] = m_v8;

	OnSelendokComboEntry();	 // to update the display.
}

void CTestControlDlg::OnFileLoadpostures() 
{
	if (!_headinitialized)
		return;

	ACE_OS::sprintf (_buffer, "%s/conf/robotcub/postures_test.txt", GetYarpRoot());
	FILE *fp = ACE_OS::fopen (_buffer, "r");
	if (fp == NULL)
	{
		MessageBox ("Can't open posture file", "Error!");
		return;
	}

	// scanf is Windows implementation, note the %lf for double.
	int i, j;
	for (i = 0; i < N_POSTURES; i++)
	{
		for (j = 0; j < _headjoints; j++)
			fscanf (fp, "%lf ", &_headstore[i][j]);
		fscanf (fp, "\n");

		for (j = 0; j < _headjoints; j++)
			fscanf (fp, "%lf ", &_headstorev[i][j]);
		fscanf (fp, "\n");
	}

	ACE_OS::fclose (fp);

	OnSelendokComboEntry();	 // to update display.
}

void CTestControlDlg::OnUpdateFileLoadpostures(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized);
}

void CTestControlDlg::OnFileSavepostures() 
{
	if (!_headinitialized)
		return;

	ACE_OS::sprintf (_buffer, "%s/conf/robotcub/postures_test.txt", GetYarpRoot());
	FILE *fp = ACE_OS::fopen (_buffer, "w");
	if (fp == NULL)
	{
		MessageBox ("Can't save posture file", "Error!");
		return;
	}

	int i, j;
	for (i = 0; i < N_POSTURES; i++)
	{
		for (j = 0; j < _headjoints; j++)
			ACE_OS::fprintf (fp, "%.2f ", _headstore[i][j]);
		ACE_OS::fprintf (fp, "\n");

		for (j = 0; j < _headjoints; j++)
			ACE_OS::fprintf (fp, "%.2f ", _headstorev[i][j]);
		ACE_OS::fprintf (fp, "\n");
	}

	ACE_OS::fclose (fp);
}

void CTestControlDlg::OnUpdateFileSavepostures(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized);
}

void CTestControlDlg::OnButtonCalibratehead() 
{
	_calibrationdlg.ShowWindow (SW_SHOW);	
}

void CTestControlDlg::OnButton0encoders() 
{
	// safely disables amplifiers first!
	head.idleMode ();
	head.resetEncoders ();
	ACE_OS::memset (_headjointstore, 0, sizeof(double) * _headjoints);
	head.setCommands (_headjointstore);
	//head.activatePID (); and it leaves it disabled.
}
