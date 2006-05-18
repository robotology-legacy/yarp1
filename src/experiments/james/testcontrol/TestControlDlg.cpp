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
YARPEsdDaqDeviceDriver touch;


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
static int xprintf_arm(const char *fmt, ...)
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
	WriteConsole(__console_handle, "ARM: ", 5, &cCharsWritten, NULL);
	WriteConsole(__console_handle, s, ACE_OS::strlen(s), &cCharsWritten, NULL);

	return(cnt);
}

/// Use wprintf like TRACE0, TRACE1, ... (The arguments are the same as printf)
static int xprintf_head(const char *fmt, ...)
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
	WriteConsole(__console_handle, "HEAD: ", 6, &cCharsWritten, NULL);
	WriteConsole(__console_handle, s, ACE_OS::strlen(s), &cCharsWritten, NULL);

	return(cnt);
}

///
static int xprintf_touch(const char *fmt, ...)
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
	WriteConsole(__console_handle, "TOUCH: ", 6, &cCharsWritten, NULL);
	WriteConsole(__console_handle, s, ACE_OS::strlen(s), &cCharsWritten, NULL);

	return(cnt);
}


static FILE *	__file_handle = NULL;

static BOOL init_log (void)
{
	if (__file_handle == NULL)
	{
		__file_handle = fopen ("log.txt", "w");
		if (__file_handle != NULL)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

static BOOL close_log (void)
{
	if (__file_handle != NULL)
		fclose (__file_handle);
	__file_handle = NULL;

	return TRUE;
}

/// Use wprintf like TRACE0, TRACE1, ... (The arguments are the same as printf)
static int xfprintf(char *fmt, ...)
{
	if (__file_handle == NULL)
		return -1;

	char s[300];
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);

	fprintf (__file_handle, "%s", s);
	return(cnt);
}


#define INITLOG init_console
#define CLOSELOG close_console
#define PRINTLOGH xprintf_head
#define PRINTLOGA xprintf_arm
#define PRINTLOGT xprintf_touch


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
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	ACE_OS::memset (m_p, 0, sizeof(double) * MAX_HEAD_JNTS);
	ACE_OS::memset (m_v, 0, sizeof(double) * MAX_HEAD_JNTS);

	ACE_OS::memset (m_pa, 0, sizeof(double) * MAX_ARM_JNTS);
	ACE_OS::memset (m_va, 0, sizeof(double) * MAX_ARM_JNTS);

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_FACE);

	_headinitialized = false;
	_headjointstore = NULL;
	_headlastreached = NULL;

	_arminitialized = false;
	_armjointstore = NULL;
	_armlastreached = NULL;

	_headrunning = false;
	_armrunning = false;

	_touchinitialized = false;
	_touchrunning = false;

	int i;
	for (i = 0; i < N_POSTURES; i++)
	{
		_headstore[i] = NULL;
		_headstorev[i] = NULL;
		_armstore[i] = NULL;
		_armstorev[i] = NULL;
	}
}

void CTestControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int i;
	for (i = 0; i < MAX_ARM_JNTS; i++)
	{
		DDX_Control(pDX, IDC_EDIT_SA1+i, m_sa_ctrl[i]);
		DDX_Control(pDX, IDC_EDIT_VA1+i, m_va_ctrl[i]);
		DDX_Control(pDX, IDC_EDIT_PA1+i, m_pa_ctrl[i]);
		DDX_Text(pDX, IDC_EDIT_PA1+i, m_pa[i]);
		DDX_Text(pDX, IDC_EDIT_VA1+i, m_va[i]);
		DDV_MinMaxDouble(pDX, m_va[i], -32768., 32767.);
	}

	for (i = 0; i < MAX_HEAD_JNTS; i++)
	{
		DDX_Control(pDX, IDC_EDIT_S1+i, m_s_ctrl[i]);
		DDX_Control(pDX, IDC_EDIT_V1+i, m_v_ctrl[i]);
		DDX_Control(pDX, IDC_EDIT_P1+i, m_p_ctrl[i]);
		DDX_Text(pDX, IDC_EDIT_P1+i, m_p[i]);
		DDX_Text(pDX, IDC_EDIT_V1+i, m_v[i]);
		DDV_MinMaxDouble(pDX, m_v[i], -32768., 32767.);
	}

	//{{AFX_DATA_MAP(CTestControlDlg)
	DDX_Control(pDX, IDC_EDIT_FAULT, m_edit_fault);
	DDX_Control(pDX, IDC_EDIT_FAULTARM, m_edit_faultarm);
	DDX_Control(pDX, IDC_COMBO_ENTRY_ALL, m_entry_all_ctrl);
	DDX_Control(pDX, IDC_BUTTON_ALL, m_goall_ctrl);
	DDX_Control(pDX, IDC_COMBO_ENTRY_ARM, m_entry_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_STORE_CURRENT_ARM, m_storecurrent_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_STORE_ARM, m_store_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_STOP_ARM, m_stop_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_RUN_ARM, m_run_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_IDLE_ARM, m_idle_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_GO_ARM, m_go_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_0ENCODERS_ARM, m_0encoders_ctrl_arm);
	DDX_Control(pDX, IDC_BUTTON_0ENCODERS, m_0encoders_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STORE_CURRENT, m_storecurrent_ctrl);
	DDX_Control(pDX, IDC_BUTTON_GO, m_go_ctrl);
	DDX_Control(pDX, IDC_COMBO_ENTRY, m_entry_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STORE, m_store_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_stop_ctrl);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_run_ctrl);
	DDX_Control(pDX, IDC_BUTTON_IDLE, m_idle_ctrl);
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
	ON_BN_CLICKED(IDC_BUTTON_0ENCODERS, OnButton0encoders)
	ON_BN_CLICKED(IDC_BUTTON_0ENCODERS_ARM, OnButton0encodersArm)
	ON_BN_CLICKED(IDC_BUTTON_GO_ARM, OnButtonGoArm)
	ON_BN_CLICKED(IDC_BUTTON_IDLE_ARM, OnButtonIdleArm)
	ON_BN_CLICKED(IDC_BUTTON_RUN_ARM, OnButtonRunArm)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ARM, OnButtonStopArm)
	ON_BN_CLICKED(IDC_BUTTON_STORE_ARM, OnButtonStoreArm)
	ON_BN_CLICKED(IDC_BUTTON_STORE_CURRENT_ARM, OnButtonStoreCurrentArm)
	ON_CBN_SELENDOK(IDC_COMBO_ENTRY_ARM, OnSelendokComboEntryArm)
	ON_BN_CLICKED(IDC_BUTTON_ALL, OnButtonAll)
	ON_CBN_SELENDOK(IDC_COMBO_ENTRY_ALL, OnSelendokComboEntryAll)
	ON_COMMAND(ID_POSTURES_SETSEQUENCE, OnPosturesSetsequence)
	ON_UPDATE_COMMAND_UI(ID_POSTURES_SETSEQUENCE, OnUpdatePosturesSetsequence)
	ON_COMMAND(ID_FILE_LOADSEQUENCE, OnFileLoadsequence)
	ON_COMMAND(ID_FILE_SAVESEQUENCE, OnFileSavesequence)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOADSEQUENCE, OnUpdateFileLoadsequence)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVESEQUENCE, OnUpdateFileSavesequence)
	ON_COMMAND(ID_INTERFACE_HIDEHALLEFFECT, OnInterfaceHidehalleffect)
	ON_UPDATE_COMMAND_UI(ID_INTERFACE_HIDEHALLEFFECT, OnUpdateInterfaceHidehalleffect)
	ON_COMMAND(ID_INTERFACE_SHOWHALLEFFECT, OnInterfaceShowhalleffect)
	ON_UPDATE_COMMAND_UI(ID_INTERFACE_SHOWHALLEFFECT, OnUpdateInterfaceShowhalleffect)
	ON_UPDATE_COMMAND_UI(ID_INTERFACE_HIDEGAIN, OnUpdateInterfaceHidegain)
	ON_UPDATE_COMMAND_UI(ID_INTERFACE_SHOWGAIN, OnUpdateInterfaceShowgain)
	ON_COMMAND(ID_INTERFACE_CALIBRATIONPANEL, OnInterfaceCalibrationpanel)
	ON_UPDATE_COMMAND_UI(ID_INTERFACE_CALIBRATIONPANEL, OnUpdateInterfaceCalibrationpanel)
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
	_calibrationdlg.Create (CCalibrationDlg::IDD, this);
	_sequencedlg.Create (CSeqDlg::IDD, this);
	_touchdlg.Create (CTouchDlg::IDD, this);

	// sort of initialization.
	m_entry_ctrl.SetCurSel(0);
	m_entry_ctrl_arm.SetCurSel(0);

	int i;
	for (i = 0; i < MAX_HEAD_JNTS; i++)
	{
		m_p_ctrl[i].SetWindowText ("0");
		m_v_ctrl[i].SetWindowText ("0");
	}

	for (i = 0; i < MAX_ARM_JNTS; i++)
	{
		m_pa_ctrl[i].SetWindowText ("0");
		m_va_ctrl[i].SetWindowText ("0");
	}

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
	if (!_headinitialized && !_arminitialized && !_touchinitialized)
		return;

	SetTimer (TIMER_ID, GUI_REFRESH_INTERVAL, NULL); 

	_gaincontroldlg.EnableInterface();
	_sequencedlg.EnableInterface();
	_touchdlg.EnableInterface();

    if (_headrunning && _armrunning)
	    _calibrationdlg.EnableInterface();

	if (_headinitialized)
	{
		m_entry_ctrl.EnableWindow();
		m_go_ctrl.EnableWindow();
		m_idle_ctrl.EnableWindow();
		m_run_ctrl.EnableWindow();
		m_stop_ctrl.EnableWindow();
		m_store_ctrl.EnableWindow();
		m_storecurrent_ctrl.EnableWindow();

		for (int i = 0; i < MAX_HEAD_JNTS; i++)
		{
			m_p_ctrl[i].EnableWindow();
			m_v_ctrl[i].EnableWindow();
		}

		m_0encoders_ctrl.EnableWindow();
		m_edit_fault.EnableWindow();
	}

	if (_arminitialized)
	{
		m_entry_ctrl_arm.EnableWindow();
		m_go_ctrl_arm.EnableWindow();
		m_idle_ctrl_arm.EnableWindow();
		m_run_ctrl_arm.EnableWindow();
		m_stop_ctrl_arm.EnableWindow();
		m_store_ctrl_arm.EnableWindow();
		m_storecurrent_ctrl_arm.EnableWindow();

		for (int i = 0; i < MAX_ARM_JNTS; i++)
		{
			m_pa_ctrl[i].EnableWindow();
			m_va_ctrl[i].EnableWindow();
		}

		m_0encoders_ctrl_arm.EnableWindow();
		m_edit_faultarm.EnableWindow();
	}

	if (_headinitialized && _arminitialized)
	{
		m_goall_ctrl.EnableWindow();
		m_entry_all_ctrl.EnableWindow();
	}
}

void CTestControlDlg::DisableGUI ()
{
	// head controls.
	m_entry_ctrl.EnableWindow(FALSE);
	m_go_ctrl.EnableWindow(FALSE);
	m_idle_ctrl.EnableWindow(FALSE);
	m_run_ctrl.EnableWindow(FALSE);
	m_stop_ctrl.EnableWindow(FALSE);
	m_store_ctrl.EnableWindow(FALSE);
	m_storecurrent_ctrl.EnableWindow(FALSE);

	int i;
	for (i = 0; i < MAX_HEAD_JNTS; i++)
	{
		m_p_ctrl[i].EnableWindow(FALSE);
		m_v_ctrl[i].EnableWindow(FALSE);
	}

	m_0encoders_ctrl.EnableWindow(FALSE);
	m_edit_fault.EnableWindow(FALSE);
	m_edit_faultarm.EnableWindow(FALSE);

	// arm controls.
	m_entry_ctrl_arm.EnableWindow(FALSE);
	m_go_ctrl_arm.EnableWindow(FALSE);
	m_idle_ctrl_arm.EnableWindow(FALSE);
	m_run_ctrl_arm.EnableWindow(FALSE);
	m_stop_ctrl_arm.EnableWindow(FALSE);
	m_store_ctrl_arm.EnableWindow(FALSE);
	m_storecurrent_ctrl_arm.EnableWindow(FALSE);

	for (i = 0; i < MAX_ARM_JNTS; i++)
	{
		m_pa_ctrl[i].EnableWindow(FALSE);
		m_va_ctrl[i].EnableWindow(FALSE);
	}

	m_0encoders_ctrl_arm.EnableWindow(FALSE);

	m_goall_ctrl.EnableWindow(FALSE);
	m_entry_all_ctrl.EnableWindow(FALSE);

	// others.
	_touchdlg.DisableInterface();
	_gaincontroldlg.DisableInterface();
	_calibrationdlg.DisableInterface();
	_sequencedlg.DisableInterface();

	KillTimer (TIMER_ID);
}

void CTestControlDlg::AllocHeadArrays(int nj)
{
	if (nj <= 0)
		return;

	_headjointstore = new double[nj];
	ACE_ASSERT (_headjointstore != NULL);
	ACE_OS::memset (_headjointstore, 0, sizeof(double) * nj);

	_headlastreached = new double[nj];
	ACE_ASSERT (_headlastreached != NULL);
	ACE_OS::memset (_headlastreached, 0, sizeof(double) * nj);

	_headfaults = new short[nj];
    ACE_ASSERT (_headfaults != NULL);
	ACE_OS::memset (_headfaults, 0, sizeof(short) * nj);

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

void CTestControlDlg::AllocArmArrays(int nj)
{
	if (nj <= 0)
		return;

	_armjointstore = new double[nj];
	ACE_ASSERT (_armjointstore != NULL);
	ACE_OS::memset (_armjointstore, 0, sizeof(double) * nj);

	_armlastreached = new double[nj];
	ACE_ASSERT (_armlastreached != NULL);
	ACE_OS::memset (_armlastreached, 0, sizeof(double) * nj);

	_armfaults = new short[nj];
    ACE_ASSERT (_armfaults != NULL);
	ACE_OS::memset (_armfaults, 0, sizeof(short) * nj);

	int i;
	for (i = 0; i < N_POSTURES; i++)
	{
		_armstore[i] = new double[nj];
		_armstorev[i] = new double[nj];
		ACE_ASSERT (_armstore[i] != NULL && _armstorev[i]);

		ACE_OS::memset (_armstore[i], 0, sizeof(double) * nj);
		ACE_OS::memset (_armstorev[i], 0, sizeof(double) * nj);
	}
}

void CTestControlDlg::FreeHeadArrays(void)
{
	if (_headfaults != NULL) delete[] _headfaults;
	_headfaults = NULL;

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

void CTestControlDlg::FreeArmArrays(void)
{
	if (_armfaults != NULL) delete[] _armfaults;
	_armfaults = NULL;

    if (_armjointstore != NULL) delete[] _armjointstore;
	_armjointstore = NULL;

	if (_armlastreached != NULL) delete[] _armlastreached;
	_armlastreached = NULL;

	int i;
	for (i = 0; i < N_POSTURES; i++)
	{
		if (_armstore[i] != NULL) delete[] _armstore[i];
		_armstore[i] = NULL;
		if (_armstorev[i] != NULL) delete[] _armstorev[i];
		_armstorev[i] = NULL;
	}
}

void CTestControlDlg::OnInterfaceStart() 
{
	char *root = GetYarpRoot();
	char path[512];

	ACE_OS::sprintf (path, "%s/%s\0", root, ConfigFilePath); 

	// initialize head controller on bus 1.
	YARPHeadParams parameters;
	parameters.load (YARPString(path), YARPString(HEAD_INI_FILE));
	parameters._message_filter = 20;
	parameters._p = PRINTLOGH;

	int ret = head.initialize(parameters);
	if (ret != YARP_OK)
	{
		_headinitialized = false;
		//DisableGUI ();
		char message[512];
		ACE_OS::sprintf (message, "Can't start the interface with the robot head, please make sure the hardware and control cards are on.\nThe ini file was: %s%s", path, HEAD_INI_FILE);
		MessageBox (message, "Error!");
		//return;
	}
	else
	{
		_headinitialized = true;
		_headrunning = false;
		ACE_ASSERT (head.nj() == MAX_HEAD_JNTS); // not other size is allowed.
	}

	// initialize arm controller on bus 2.
	YARPArmParams aparameters;
	aparameters.load (YARPString(path), YARPString(ARM_INI_FILE));
	aparameters._message_filter = 20;
	aparameters._p = PRINTLOGA;

	ret = arm.initialize(aparameters);
	if (ret != YARP_OK)
	{
		_arminitialized = false;
		//DisableGUI ();
		char message[512];
		ACE_OS::sprintf (message, "Can't start the interface with the robot arm/hand, please make sure the hardware and control cards are on.\nThe ini file was: %s%s", path, ARM_INI_FILE);
		MessageBox (message, "Error!");
		//return;
	}
	else
	{
		_arminitialized = true;
		_armrunning = false;
		ACE_ASSERT (arm.nj() == MAX_ARM_JNTS);
	}

	// direct initialization of the hall-effect mph-daq card device driver.
	EsdDaqOpenParameters op_par;
	op_par._networkN = 1;				// same address of the arm/hand. LATER: SET THE CORRECT BUS NUMBER HERE!
	op_par._remote_address = 5;			// address of the daq card.
	op_par._my_address = 0;				// this is the second instance to the same driver (we can even use a different ID).
	op_par._polling_interval = 10;
	op_par._timeout = 10;			
	//op_par._scanSequence = 0xffffffff; //0x03ffefff; // first 7 channels not connected.
    //op_par._broadcast = true;
    int scansequence = 0xffffffff;
	_touchdlg.SetMask (scansequence);

	if (touch.open ((void *)&op_par) != YARP_OK)
	{
		touch.close();

		char message[512];
		ACE_OS::sprintf (message, "Can't start the interface with the hall-effect sensor card, please make sure the hardware and control cards are on.");
		MessageBox (message, "Error!");
	}
	else
	{
		_touchinitialized = true;

		/*
		int ret = touch.IOCtl (CMDGetMaxChannels, (void *)&_maxanalogchannels);
		if (_maxanalogchannels)
		{
			_analogstore = new short[_maxanalogchannels];
			ACE_ASSERT (_analogstore != NULL);
			ACE_OS::memset (_analogstore, 0, sizeof(short) * _maxanalogchannels);
		}
		*/

        touch.IOCtl (CMDBroadcastSetup, &scansequence);
		_touchrunning = true;
	}	

	if (_headinitialized)
		AllocHeadArrays (MAX_HEAD_JNTS);

	if (_arminitialized)
		AllocArmArrays (MAX_ARM_JNTS);

	EnableGUI ();
}

void CTestControlDlg::OnInterfaceStop() 
{
	CLOSELOG();

	if (_headinitialized)
	{
		head.idleMode ();
		_headrunning = false;
		head.uninitialize ();	
		FreeHeadArrays ();
		_headinitialized = false;
	}

	if (_arminitialized)
	{
		arm.idleMode ();
		_armrunning = false;
		arm.uninitialize ();	
		FreeArmArrays ();
		_arminitialized = false;
	}
	
	if (_touchinitialized)
	{
        // stop broadcast.
        int x = 0;
        touch.IOCtl (CMDScanSetup, &x);

		touch.close ();
		_touchrunning = false;
		_touchinitialized = false;
	}

	DisableGUI ();
}

void CTestControlDlg::OnUpdateInterfaceStart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!_headinitialized && !_arminitialized && !_touchinitialized);
}

void CTestControlDlg::OnUpdateInterfaceStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized || _arminitialized || _touchinitialized);
}

void CTestControlDlg::OnFileOpenconsole() 
{
	if (INITLOG())
	{
		PRINTLOGH("Debug window started...\n");
		PRINTLOGA("Debug window started...\n");
		PRINTLOGT("Debug window started...\n");
	}
}

void CTestControlDlg::OnUpdateFileOpenconsole(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized || _arminitialized || _touchinitialized);
}

void CTestControlDlg::OnFileCloseconsole() 
{
	CLOSELOG();
}

void CTestControlDlg::OnUpdateFileCloseconsole(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized || _arminitialized || _touchinitialized);
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
	if (head.activatePID (false) == YARP_OK)
    {
		_headrunning = true;
        if (_armrunning)
            _calibrationdlg.EnableInterface();
    }
}

void CTestControlDlg::OnButtonRunArm() 
{
	if (arm.activatePID (false) == YARP_OK)
    {
		_armrunning = true;
        if (_headrunning)
            _calibrationdlg.EnableInterface();
    }
}

void CTestControlDlg::OnButtonIdle() 
{
	if (head.idleMode () == YARP_OK)
    {
		_headrunning = false;
        _calibrationdlg.DisableInterface();
    }
}

void CTestControlDlg::OnButtonIdleArm() 
{
	if (arm.idleMode () == YARP_OK)
    {
		_armrunning = false;
        _calibrationdlg.DisableInterface();
    }
}

void CTestControlDlg::OnTimer(UINT nIDEvent) 
{
	if (_headinitialized)
	{
		head.getPositions (_headjointstore);
		ACE_OS::memcpy (_headlastreached, _headjointstore, sizeof(double) * MAX_HEAD_JNTS);
		int i;
		for (i = 0; i < MAX_HEAD_JNTS; i++)
		{
			ACE_OS::sprintf (_buffer, "%.2f", _headjointstore[i]);
			m_s_ctrl[i].SetWindowText (_buffer);
		}

		head.getFaults (_headfaults);
		for (i = 0; i < MAX_HEAD_JNTS; i++)
		{
            if (_headfaults[i] != 0)
                _buffer[i] = '1';
            else
                _buffer[i] = '0';
		}
        _buffer[i] = 0;
		m_edit_fault.SetWindowText (_buffer);
	}
	
	if (_arminitialized)
	{
		arm.getPositions (_armjointstore);
		ACE_OS::memcpy (_armlastreached, _armjointstore, sizeof(double) * MAX_ARM_JNTS);

		for (int i = 0; i < MAX_ARM_JNTS; i++)
		{
			ACE_OS::sprintf (_buffer, "%.2f", _armjointstore[i]);
			m_sa_ctrl[i].SetWindowText (_buffer);
		}

        arm.getFaults (_armfaults);
		for (i = 0; i < MAX_ARM_JNTS; i++)
		{
            if (_armfaults[i] != 0)
                _buffer[i] = '1';
            else
                _buffer[i] = '0';
		}
        _buffer[i] = 0;
		m_edit_faultarm.SetWindowText (_buffer);
	}

	if (_touchinitialized)
	{
		if (_touchdlg.IsWindowVisible())
		{
			_touchdlg.UpdateInterface();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CTestControlDlg::OnButtonGo() 
{
	if (_headrunning)
	{
		UpdateData (TRUE);

		ACE_OS::memcpy (_headjointstore, m_v, sizeof(double) * MAX_HEAD_JNTS);
		head.setVelocities (_headjointstore);

		ACE_OS::memcpy (_headjointstore, m_p, sizeof(double) * MAX_HEAD_JNTS);
		head.setPositions (_headjointstore);
	}
	else
		MessageBox ("The controller must be running before commanding the robot", "Error!");
}

void CTestControlDlg::OnButtonGoArm() 
{
	if (_armrunning)
	{
		UpdateData (TRUE);

		ACE_OS::memcpy (_armjointstore, m_va, sizeof(double) * MAX_ARM_JNTS);
		arm.setVelocities (_armjointstore);

		ACE_OS::memcpy (_armjointstore, m_pa, sizeof(double) * MAX_ARM_JNTS);
		arm.setPositions (_armjointstore);
	}
	else
		MessageBox ("The controller must be running before commanding the robot", "Error!");
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

	ACE_OS::memcpy (_headstore[entry], m_p, sizeof(double) * MAX_HEAD_JNTS);
	ACE_OS::memcpy (_headstorev[entry], m_v, sizeof(double) * MAX_HEAD_JNTS);
}

void CTestControlDlg::OnButtonStoreArm() 
{
	// saves the current displayed position into the store.
	UpdateData (TRUE);

	const int entry = m_entry_ctrl_arm.GetCurSel();	

	if (entry == CB_ERR || entry < 0 || entry >= N_POSTURES)
	{
		MessageBox ("Invalid selection: don't know where to save data", "Error!");
		return;
	}

	ACE_OS::memcpy (_armstore[entry], m_pa, sizeof(double) * MAX_ARM_JNTS);
	ACE_OS::memcpy (_armstorev[entry], m_va, sizeof(double) * MAX_ARM_JNTS);
}

void CTestControlDlg::OnSelendokComboEntry() 
{
	const int entry = m_entry_ctrl.GetCurSel();	

	if (entry == CB_ERR || entry < 0 || entry >= N_POSTURES)
	{
		MessageBox ("Invalid selection", "Error!");
		return;
	}

	ACE_OS::memcpy (m_p, _headstore[entry], sizeof(double) * MAX_HEAD_JNTS);
	ACE_OS::memcpy (m_v, _headstorev[entry], sizeof(double) * MAX_HEAD_JNTS);

	UpdateData(FALSE);
}

void CTestControlDlg::OnSelendokComboEntryArm() 
{
	const int entry = m_entry_ctrl_arm.GetCurSel();	

	if (entry == CB_ERR || entry < 0 || entry >= N_POSTURES)
	{
		MessageBox ("Invalid selection", "Error!");
		return;
	}

	ACE_OS::memcpy (m_pa, _armstore[entry], sizeof(double) * MAX_ARM_JNTS);
	ACE_OS::memcpy (m_va, _armstorev[entry], sizeof(double) * MAX_ARM_JNTS);

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

	ACE_OS::memcpy (_headstore[entry], _headlastreached, sizeof(double) * MAX_HEAD_JNTS);
	ACE_OS::memcpy (_headstorev[entry], m_v, sizeof(double) * MAX_HEAD_JNTS);
	
	OnSelendokComboEntry();	 // to update the display.
}

void CTestControlDlg::OnButtonStoreCurrentArm() 
{
	// saves the current displayed position into the store.
	UpdateData (TRUE);

	const int entry = m_entry_ctrl_arm.GetCurSel();	

	if (entry == CB_ERR || entry < 0 || entry >= N_POSTURES)
	{
		MessageBox ("Invalid selection: don't know where to save data", "Error!");
		return;
	}

	ACE_OS::memcpy (_armstore[entry], _armlastreached, sizeof(double) * MAX_ARM_JNTS);
	ACE_OS::memcpy (_armstorev[entry], m_va, sizeof(double) * MAX_ARM_JNTS);

	OnSelendokComboEntryArm();	 // to update the display.
}

void CTestControlDlg::OnFileLoadpostures() 
{
	if (!_headinitialized && !_arminitialized)
		return;

	ACE_OS::sprintf (_buffer, "%s/%spostures_test.txt", GetYarpRoot(), ConfigFilePath);
	FILE *fp = ACE_OS::fopen (_buffer, "r");
	if (fp == NULL)
	{
		MessageBox ("Can't open posture file", "Error!");
		return;
	}

	// scanf is Windows implementation, note the %lf for reading doubles.

	if (_headinitialized)
	{
		int i, j;
		for (i = 0; i < N_POSTURES; i++)
		{
			for (j = 0; j < MAX_HEAD_JNTS; j++)
				fscanf (fp, "%lf ", &_headstore[i][j]);
			fscanf (fp, "\n");

			for (j = 0; j < MAX_HEAD_JNTS; j++)
				fscanf (fp, "%lf ", &_headstorev[i][j]);
			fscanf (fp, "\n");
		}
	
		OnSelendokComboEntry();	 // to update display.
	}
	else
	{
		// to skip the head entries.
		int i, j;
		double dummy;
		for (i = 0; i < N_POSTURES; i++)
		{
			for (j = 0; j < MAX_HEAD_JNTS; j++)
				fscanf (fp, "%lf ", &dummy);
			fscanf (fp, "\n");

			for (j = 0; j < MAX_HEAD_JNTS; j++)
				fscanf (fp, "%lf ", &dummy);
			fscanf (fp, "\n");
		}
	}

	if (_arminitialized)
	{
		int i, j;
		for (i = 0; i < N_POSTURES; i++)
		{
			for (j = 0; j < MAX_ARM_JNTS; j++)
				fscanf (fp, "%lf ", &_armstore[i][j]);
			fscanf (fp, "\n");

			for (j = 0; j < MAX_ARM_JNTS; j++)
				fscanf (fp, "%lf ", &_armstorev[i][j]);
			fscanf (fp, "\n");
		}

		OnSelendokComboEntryArm();	 // to update display.
	}

	ACE_OS::fclose (fp);
}

void CTestControlDlg::OnUpdateFileLoadpostures(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized || _arminitialized);
}

void CTestControlDlg::OnFileSavepostures() 
{
	int ret = AfxMessageBox ("Are you sure, this would overwrite existing posture file",
				   MB_ICONQUESTION | MB_YESNO);
	if (ret == 0 || ret == IDNO)
		return;
	
	if (!_headinitialized && !_arminitialized)
		return;

	ACE_OS::sprintf (_buffer, "%s/%spostures_test.txt", GetYarpRoot(), ConfigFilePath);
	FILE *fp = ACE_OS::fopen (_buffer, "w");
	if (fp == NULL)
	{
		MessageBox ("Can't save posture file", "Error!");
		return;
	}

	if (_headinitialized)
	{
		int i, j;
		for (i = 0; i < N_POSTURES; i++)
		{
			for (j = 0; j < MAX_HEAD_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", _headstore[i][j]);
			ACE_OS::fprintf (fp, "\n");

			for (j = 0; j < MAX_HEAD_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", _headstorev[i][j]);
			ACE_OS::fprintf (fp, "\n");
		}
	}
	else
	{
		int i, j;
		double dummy = 0;
		for (i = 0; i < N_POSTURES; i++)
		{
			for (j = 0; j < MAX_HEAD_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", dummy);
			ACE_OS::fprintf (fp, "\n");

			for (j = 0; j < MAX_HEAD_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", dummy);
			ACE_OS::fprintf (fp, "\n");
		}
	}

	if (_arminitialized)
	{
		int i, j;
		for (i = 0; i < N_POSTURES; i++)
		{
			for (j = 0; j < MAX_ARM_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", _armstore[i][j]);
			ACE_OS::fprintf (fp, "\n");

			for (j = 0; j < MAX_ARM_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", _armstorev[i][j]);
			ACE_OS::fprintf (fp, "\n");
		}
	}
	else
	{
		int i, j;
		double dummy = 0;
		for (i = 0; i < N_POSTURES; i++)
		{
			for (j = 0; j < MAX_ARM_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", dummy);
			ACE_OS::fprintf (fp, "\n");

			for (j = 0; j < MAX_ARM_JNTS; j++)
				ACE_OS::fprintf (fp, "%.2f ", dummy);
			ACE_OS::fprintf (fp, "\n");
		}
	}

	ACE_OS::fclose (fp);
}

void CTestControlDlg::OnUpdateFileSavepostures(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized || _arminitialized);
}

void CTestControlDlg::OnButton0encoders() 
{
	// safely disables amplifiers first!
    _calibrationdlg.DisableInterface();
	head.idleMode ();
	head.resetEncoders ();
	_headrunning = false;
	ACE_OS::memset (_headjointstore, 0, sizeof(double) * MAX_HEAD_JNTS);
	head.setCommands (_headjointstore);
}

void CTestControlDlg::OnButton0encodersArm() 
{
	// safely disables amplifiers first!
    _calibrationdlg.DisableInterface();
	arm.idleMode ();
	arm.resetEncoders ();
	_armrunning = false;
	ACE_OS::memset (_armjointstore, 0, sizeof(double) * MAX_ARM_JNTS);
	arm.setCommands (_armjointstore);
}

void CTestControlDlg::OnButtonStopArm() 
{
	// LATER: to be implemented!
}

void CTestControlDlg::OnButtonAll() 
{
	if (_headrunning && _armrunning)
	{
		UpdateData (TRUE);

		ACE_OS::memcpy (_headjointstore, m_v, sizeof(double) * MAX_HEAD_JNTS);
		head.setVelocities (_headjointstore);

		ACE_OS::memcpy (_armjointstore, m_va, sizeof(double) * MAX_ARM_JNTS);
		arm.setVelocities (_armjointstore);

		ACE_OS::memcpy (_headjointstore, m_p, sizeof(double) * MAX_HEAD_JNTS);
		head.setPositions (_headjointstore);

		ACE_OS::memcpy (_armjointstore, m_pa, sizeof(double) * MAX_ARM_JNTS);
		arm.setPositions (_armjointstore);
	}
	else
	{
		MessageBox ("The controllers [arm and head] must be running before using this button", "Error!");
	}
}

void CTestControlDlg::OnSelendokComboEntryAll() 
{
	const int index = m_entry_all_ctrl.GetCurSel();
	if (index != CB_ERR && index >= 0 && index < N_POSTURES)
	{
		m_entry_ctrl.SetCurSel (index);
		ACE_OS::memcpy (m_p, _headstore[index], sizeof(double) * MAX_HEAD_JNTS);
		ACE_OS::memcpy (m_v, _headstorev[index], sizeof(double) * MAX_HEAD_JNTS);

		m_entry_ctrl_arm.SetCurSel (index);
		ACE_OS::memcpy (m_pa, _armstore[index], sizeof(double) * MAX_ARM_JNTS);
		ACE_OS::memcpy (m_va, _armstorev[index], sizeof(double) * MAX_ARM_JNTS);

		UpdateData(FALSE);
	}
}

void CTestControlDlg::OnPosturesSetsequence() 
{
	_sequencedlg.ShowWindow(SW_SHOW);
}

void CTestControlDlg::OnUpdatePosturesSetsequence(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized || _arminitialized);
}

void CTestControlDlg::OnFileLoadsequence() 
{
	int ret = AfxMessageBox ("Are you sure, this would overwrite the current memory",
				   MB_ICONQUESTION | MB_YESNO);
	if (ret == 0 || ret == IDNO)
		return;
	
	if (!_headinitialized || !_arminitialized)
		return;

	ACE_OS::sprintf (_buffer, "%s/%ssequence_test.txt", GetYarpRoot(),ConfigFilePath);
	FILE *fp = ACE_OS::fopen (_buffer, "r");
	if (fp == NULL)
	{
		MessageBox ("Can't read sequence file", "Error!");
		return;
	}

	_sequencedlg.LoadSequence (fp);

	ACE_OS::fclose (fp);    
}

void CTestControlDlg::OnFileSavesequence() 
{
	int ret = AfxMessageBox ("Are you sure, this would overwrite the existing sequence file",
				   MB_ICONQUESTION | MB_YESNO);
	if (ret == 0 || ret == IDNO)
		return;
	
	if (!_headinitialized || !_arminitialized)
		return;

	ACE_OS::sprintf (_buffer, "%s/%ssequence_test.txt", GetYarpRoot(),ConfigFilePath);
	FILE *fp = ACE_OS::fopen (_buffer, "w");
	if (fp == NULL)
	{
		MessageBox ("Can't save sequence file", "Error!");
		return;
	}

	_sequencedlg.SaveSequence (fp);

	ACE_OS::fclose (fp);
}

void CTestControlDlg::OnUpdateFileLoadsequence(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized && _arminitialized);
}

void CTestControlDlg::OnUpdateFileSavesequence(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_headinitialized && _arminitialized);
}

void CTestControlDlg::OnInterfaceHidehalleffect() 
{
	_touchdlg.ShowWindow (SW_HIDE);
}

void CTestControlDlg::OnUpdateInterfaceHidehalleffect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_touchdlg.IsWindowVisible());
}

void CTestControlDlg::OnInterfaceShowhalleffect() 
{
	_touchdlg.ShowWindow (SW_SHOW);	
}

void CTestControlDlg::OnUpdateInterfaceShowhalleffect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!_touchdlg.IsWindowVisible());	
}

void CTestControlDlg::OnUpdateInterfaceHidegain(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (_gaincontroldlg.IsWindowVisible());
}

void CTestControlDlg::OnUpdateInterfaceShowgain(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!_gaincontroldlg.IsWindowVisible());
}

void CTestControlDlg::OnInterfaceCalibrationpanel() 
{
    _calibrationdlg.ShowWindow (SW_SHOW);
}

void CTestControlDlg::OnUpdateInterfaceCalibrationpanel(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable (_headrunning && _armrunning);	
}
