// CanControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CanControl.h"
#include "CanControlDlg.h"
#include "CanControlParams.h"
#include "CardAddressDlg.h"
#include "CardsDlg.h"
#include "SetAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///
/// helper functions.
/// 
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
// CCanControlDlg dialog

CCanControlDlg::CCanControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCanControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCanControlDlg)
	m_axis = -1;
	m_pgain = 0.0;
	m_dgain = 0.0;
	m_igain = 0.0;
	m_ilimit = 0.0;
	m_offset = 0.0;
	m_shift = 0.0;
	m_tlimit = 0.0;
	m_desired_position = 0.0;
	m_desired_speed = 0.0;
	m_desired_acceleration = 0.0;
	m_min_position = 0.0;
	m_max_position = 0.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCanControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCanControlDlg)
	DDX_Control(pDX, IDC_BUTTON_SPY, m_spy_ctrl);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_status_ctrl);
	DDX_Control(pDX, IDC_FLASH_READ, m_flash_read_ctrl);
	DDX_Control(pDX, IDC_EDIT_MAX, m_max_ctrl);
	DDX_Control(pDX, IDC_EDIT_MIN, m_min_ctrl);
	DDX_Control(pDX, IDC_BUTTON_SETMINMAX, m_setminmax_ctrl);
	DDX_Control(pDX, IDC_BUTTON_PREPARE, m_prepare_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_stop_ctrl);
	DDX_Control(pDX, IDC_EDIT_ACCELERATION, m_desired_acceleration_ctrl);
	DDX_Control(pDX, IDC_COMBO_MODE, m_mode_ctrl);
	DDX_Control(pDX, IDC_EDIT_SPEED, m_desired_speed_ctrl);
	DDX_Control(pDX, IDC_EDIT_POSITION, m_desired_position_ctrl);
	DDX_Control(pDX, IDC_BUTTON_GO, m_go_ctrl);
	DDX_Control(pDX, IDC_BUTTON_PWM_EN, m_pwm_enable_ctrl);
	DDX_Control(pDX, IDC_BUTTON_PWM_DIS, m_pwm_disable_ctrl);
	DDX_Control(pDX, IDC_BUTTON_ENABLE, m_enable_ctrl);
	DDX_Control(pDX, IDC_BUTTON_DISABLE, m_disable_ctrl);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_reset_position_ctrl);
	DDX_Control(pDX, IDC_EDIT_CURRENT_POSITION, m_current_position_ctrl);
	DDX_Control(pDX, IDC_FLASH, m_flash_ctrl);
	DDX_Control(pDX, IDC_EDIT_TLIMIT, m_tlimit_ctrl);
	DDX_Control(pDX, IDC_EDIT_SHIFT, m_shift_ctrl);
	DDX_Control(pDX, IDC_EDIT_OFFSET, m_offset_ctrl);
	DDX_Control(pDX, IDC_EDIT_ILIMIT, m_ilimit_ctrl);
	DDX_Control(pDX, IDC_EDIT_IGAIN, m_igain_ctrl);
	DDX_Control(pDX, IDC_EDIT_DGAIN, m_dgain_ctrl);
	DDX_Control(pDX, IDC_EDIT_PGAIN, m_pgain_ctrl);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_update_ctrl);
	DDX_Control(pDX, IDC_COMBO_AXIS, m_axis_ctrl);
	DDX_CBIndex(pDX, IDC_COMBO_AXIS, m_axis);
	DDX_Text(pDX, IDC_EDIT_PGAIN, m_pgain);
	DDV_MinMaxDouble(pDX, m_pgain, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_DGAIN, m_dgain);
	DDV_MinMaxDouble(pDX, m_dgain, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_IGAIN, m_igain);
	DDV_MinMaxDouble(pDX, m_igain, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_ILIMIT, m_ilimit);
	DDV_MinMaxDouble(pDX, m_ilimit, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_OFFSET, m_offset);
	DDV_MinMaxDouble(pDX, m_offset, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_SHIFT, m_shift);
	DDV_MinMaxDouble(pDX, m_shift, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_TLIMIT, m_tlimit);
	DDV_MinMaxDouble(pDX, m_tlimit, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_POSITION, m_desired_position);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_desired_speed);
	DDV_MinMaxDouble(pDX, m_desired_speed, -100., 100.);
	DDX_Text(pDX, IDC_EDIT_ACCELERATION, m_desired_acceleration);
	DDV_MinMaxDouble(pDX, m_desired_acceleration, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_MIN, m_min_position);
	DDX_Text(pDX, IDC_EDIT_MAX, m_max_position);
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
	ON_COMMAND(ID_PARAMETERS_ADDRESSOFCARDS, OnParametersAddressofcards)
	ON_COMMAND(ID_PARAMETERS_NUMBEROFCARDS, OnParametersNumberofcards)
	ON_CBN_SELENDOK(IDC_COMBO_AXIS, OnSelendokComboAxis)
	ON_UPDATE_COMMAND_UI(ID_DRIVER_KILL, OnUpdateDriverKill)
	ON_UPDATE_COMMAND_UI(ID_DRIVER_RUN, OnUpdateDriverRun)
	ON_COMMAND(ID_DRIVER_SENDADDRESS, OnDriverSendaddress)
	ON_UPDATE_COMMAND_UI(ID_DRIVER_SENDADDRESS, OnUpdateDriverSendaddress)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_BN_CLICKED(IDC_FLASH, OnFlash)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_DISABLE, OnButtonDisable)
	ON_BN_CLICKED(IDC_BUTTON_ENABLE, OnButtonEnable)
	ON_BN_CLICKED(IDC_BUTTON_PWM_DIS, OnButtonPwmDis)
	ON_BN_CLICKED(IDC_BUTTON_PWM_EN, OnButtonPwmEn)
	ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
	ON_CBN_SELENDOK(IDC_COMBO_MODE, OnSelendokComboMode)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_UPDATE_COMMAND_UI(ID_PARAMETERS_ADDRESSOFCARDS, OnUpdateParametersAddressofcards)
	ON_UPDATE_COMMAND_UI(ID_PARAMETERS_NUMBEROFCARDS, OnUpdateParametersNumberofcards)
	ON_BN_CLICKED(IDC_BUTTON_PREPARE, OnButtonPrepare)
	ON_BN_CLICKED(IDC_BUTTON_SETMINMAX, OnButtonSetminmax)
	ON_BN_CLICKED(IDC_FLASH_READ, OnFlashRead)
	ON_BN_CLICKED(IDC_BUTTON_SPY, OnButtonSpy)
	ON_COMMAND(ID_FILE_OPENCONSOLE, OnFileOpenconsole)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENCONSOLE, OnUpdateFileOpenconsole)
	ON_COMMAND(ID_FILE_CLOSECONSOLE, OnFileCloseconsole)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSECONSOLE, OnUpdateFileCloseconsole)
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

void CCanControlDlg::ActivateGUI ()
{
	m_update_ctrl.EnableWindow();
	m_axis_ctrl.EnableWindow();
	m_pgain_ctrl.EnableWindow();
	m_dgain_ctrl.EnableWindow();
	m_igain_ctrl.EnableWindow();
	m_ilimit_ctrl.EnableWindow();
	m_tlimit_ctrl.EnableWindow();
	m_offset_ctrl.EnableWindow();
	m_shift_ctrl.EnableWindow();
	m_flash_ctrl.EnableWindow();
	m_flash_read_ctrl.EnableWindow();

	m_current_position_ctrl.EnableWindow();
	m_reset_position_ctrl.EnableWindow();
	m_enable_ctrl.EnableWindow();
	m_disable_ctrl.EnableWindow();
	m_pwm_enable_ctrl.EnableWindow();
	m_pwm_disable_ctrl.EnableWindow();

	m_go_ctrl.EnableWindow();
	m_stop_ctrl.EnableWindow();
	m_prepare_ctrl.EnableWindow();
	m_desired_position_ctrl.EnableWindow();
	m_desired_speed_ctrl.EnableWindow();
	m_mode_ctrl.EnableWindow();
	m_min_ctrl.EnableWindow();
	m_max_ctrl.EnableWindow();
	m_setminmax_ctrl.EnableWindow();
	m_spy_ctrl.EnableWindow();

	/// 
	SetTimer (TIMER_ID, GUI_REFRESH_INTERVAL, NULL); 
	m_mode_ctrl.SetCurSel (0);
}

void CCanControlDlg::DeactivateGUI ()
{
	KillTimer (TIMER_ID);
	close_console ();

	m_update_ctrl.EnableWindow(FALSE);
	m_axis_ctrl.EnableWindow(FALSE);
	m_pgain_ctrl.EnableWindow(FALSE);
	m_dgain_ctrl.EnableWindow(FALSE);
	m_igain_ctrl.EnableWindow(FALSE);
	m_ilimit_ctrl.EnableWindow(FALSE);
	m_tlimit_ctrl.EnableWindow(FALSE);
	m_offset_ctrl.EnableWindow(FALSE);
	m_shift_ctrl.EnableWindow(FALSE);
	m_flash_ctrl.EnableWindow(FALSE);
	m_flash_read_ctrl.EnableWindow(FALSE);

	///
	m_current_position_ctrl.EnableWindow(FALSE);
	m_reset_position_ctrl.EnableWindow(FALSE);
	m_enable_ctrl.EnableWindow(FALSE);
	m_disable_ctrl.EnableWindow(FALSE);
	m_pwm_enable_ctrl.EnableWindow(FALSE);
	m_pwm_disable_ctrl.EnableWindow(FALSE);

	m_go_ctrl.EnableWindow(FALSE);
	m_stop_ctrl.EnableWindow(FALSE);
	m_prepare_ctrl.EnableWindow(FALSE);
	m_desired_position_ctrl.EnableWindow(FALSE);
	m_desired_speed_ctrl.EnableWindow(FALSE);
	m_min_ctrl.EnableWindow(FALSE);
	m_max_ctrl.EnableWindow(FALSE);
	m_setminmax_ctrl.EnableWindow(FALSE);
	m_spy_ctrl.EnableWindow(FALSE);

	m_mode_ctrl.EnableWindow(FALSE);
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
	m_params._p = xprintf;

	if (m_driver.open ((void *)&m_params) < 0)
	{
		m_driverok = false;
		m_driver.close ();
		MessageBox ("The driver didn't start correctly, pls check the hardware", "Error!");
		return;
	}

	MessageBox ("The driver is now running", "Good!");
	m_driverok = true;
	ActivateGUI ();
}

void CCanControlDlg::OnDriverKill() 
{
	if (m_driverok)
	{
		m_driver.close ();
		m_driverok = false;

		DeactivateGUI ();
	}
}

void CCanControlDlg::OnParametersAddressofcards() 
{
	CCardAddressDlg dlgAddress;
	int i;

	for (i = 0; i < CANBUS_MAXCARDS; i++)
		dlgAddress.m_address[i] = m_destinations[i];

	int nResponse = dlgAddress.DoModal();

	if (nResponse == IDOK)
	{
		for (i = 0; i < CANBUS_MAXCARDS-1; i++)
		{
			m_destinations[i] = dlgAddress.m_address[i];
		}
	}	
}

void CCanControlDlg::OnParametersNumberofcards() 
{
	CCardsDlg dlgCards;
	dlgCards.m_cards = m_njoints/2;
	int nResponse = dlgCards.DoModal();

	if (nResponse == IDOK)
	{
		m_njoints = dlgCards.m_cards * 2;
	}	
}

void CCanControlDlg::UpdateAxisParams (int axis)
{
	LowLevelPID pid;
	SingleAxisParameters par;
	par.parameters = (void *)&pid;
	par.axis = axis;
	m_driver.IOCtl(CMDGetPID, (void *)&par);

	m_pgain = pid.KP;
	m_dgain = pid.KD;
	m_shift = pid.SHIFT;
	m_offset = pid.OFFSET;
	m_ilimit = pid.I_LIMIT;
	m_igain = pid.KI;
	m_tlimit = pid.T_LIMIT;
	UpdateData(FALSE);
}

/// sends controller gains.
void CCanControlDlg::OnButtonUpdate() 
{
	UpdateData(TRUE);

	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{	
		LowLevelPID pid;
		SingleAxisParameters par;
		par.parameters = (void *)&pid;
		par.axis = index;
		
		pid.KP = m_pgain;
		pid.KD = m_dgain;
		pid.SHIFT = m_shift;
		pid.OFFSET = m_offset;
		pid.I_LIMIT = m_ilimit;
		pid.KI = m_igain;
		pid.T_LIMIT = m_tlimit;

		m_driver.IOCtl(CMDSetPID, (void *)&par);
	}	
}

void CCanControlDlg::OnSelendokComboAxis() 
{
	int index = m_axis_ctrl.GetCurSel();

	if (index >= m_njoints || index == CB_ERR)
	{
		m_axis_ctrl.SetCurSel(-1);
	}
	else
	{
		UpdateAxisParams (index);
		m_axis_ctrl.SetCurSel(index);
	}
}

void CCanControlDlg::OnUpdateDriverKill(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_driverok);
}

void CCanControlDlg::OnUpdateDriverRun(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_driverok);
}

void CCanControlDlg::OnDriverSendaddress() 
{
	CSetAddressDlg dlgAddr;
	dlgAddr.m_address = 0;
	int nResponse = dlgAddr.DoModal();

	if (nResponse == IDOK)
	{
		///dlgAddr.m_address;
	}	
}

void CCanControlDlg::OnUpdateDriverSendaddress(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);
}


void CCanControlDlg::OnFlash() 
{
	m_driver.IOCtl(CMDSaveBootMemory, NULL);	
}

void CCanControlDlg::OnFlashRead() 
{
	m_driver.IOCtl(CMDLoadBootMemory, NULL);
}

void CCanControlDlg::OnTimer(UINT nIDEvent) 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index < m_njoints && index != CB_ERR)
	{
		double tmp;
		SingleAxisParameters par;
		par.parameters = (void *)&tmp;
		par.axis = index;

		int ret = m_driver.IOCtl(CMDGetPosition, (void *)&par);
		ACE_OS::sprintf (m_buffer, "%f", *((double *)par.parameters));
		m_current_position_ctrl.SetWindowText (m_buffer);

		if (ret != YARP_OK)
		{
			m_status_ctrl.SetWindowText ("Error!");
		}
		else
		{
			m_status_ctrl.SetWindowText ("Ok");
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CCanControlDlg::OnButtonReset() 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		m_driver.IOCtl(CMDDisableAmp, &index);
		m_driver.IOCtl(CMDControllerIdle, &index);

		double tmp = 0;
		SingleAxisParameters par;
		par.parameters = (void *)&tmp;
		par.axis = index;

		m_driver.IOCtl(CMDDefinePosition, (void *)&par);
		m_driver.IOCtl(CMDSetCommand, (void *)&par);

		MessageBox ("Encoder is zeroed, amplifier disabled", "Info");
	}
}

void CCanControlDlg::OnButtonDisable() 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		m_driver.IOCtl(CMDControllerIdle, &index);
	}
}

void CCanControlDlg::OnButtonEnable() 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		m_driver.IOCtl(CMDControllerRun, &index);
	}
}

void CCanControlDlg::OnButtonPwmDis() 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		m_driver.IOCtl(CMDDisableAmp, &index);
	}
}

void CCanControlDlg::OnButtonPwmEn() 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		m_driver.IOCtl(CMDEnableAmp, &index);
	}
}

void CCanControlDlg::OnButtonGo() 
{
	UpdateData(TRUE);

	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		switch (m_mode_ctrl.GetCurSel())
		{
		case 0:
			{
				SingleAxisParameters x;
				x.axis = index;				
				double params[2] = { 0, 0 };
				x.parameters = (void *)params;

				params[0] = m_desired_position;
				params[1] = m_desired_speed;

				if (params[1] < 1)
					params[1] = 1;

				m_driver.IOCtl(CMDSetPosition, (void *)&x);
			}
			break;

		case 1:
			{
				double cmd[4] = { 0, 0, 0, 0 };
				if ((index % 2) == 0)
				{
					cmd[0] = m_desired_speed;
					cmd[1] = m_desired_acceleration;
					if (cmd[1] < 1)
						cmd[1] = 1;
					cmd[2] = cmd[3] = 0;
				}
				else
				{
					cmd[0] = cmd[1] = 0;
					cmd[2] = m_desired_speed;
					cmd[3] = m_desired_acceleration;
					if (cmd[3] < 1)
						cmd[3] = 1;
				}

				m_driver.IOCtl(CMDVMove, (void *)cmd);
			}
			break;
		}
	}
}

void CCanControlDlg::OnSelendokComboMode() 
{
	switch (m_mode_ctrl.GetCurSel())
	{
	case 0:
		/// position mode.
		m_desired_acceleration_ctrl.EnableWindow (FALSE);
		m_desired_position_ctrl.EnableWindow ();

		break;

	case 1:
		/// velocity mode.
		m_desired_acceleration_ctrl.EnableWindow ();
		m_desired_position_ctrl.EnableWindow (FALSE);

		break;
	}
}

void CCanControlDlg::OnButtonStop() 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		switch (m_mode_ctrl.GetCurSel())
		{
		case 0:
			{
				/// LATER: implement the abort trajectory command.
				MessageBox ("Can't abort trajectory generation", "Not implemented!");
			}
			break;

		case 1:
			{
				double cmd[4] = { 0, 1, 0, 1 };
				if ((index % 2) == 0)
				{
					cmd[0] = 0;
					cmd[1] = m_desired_acceleration;
					if (cmd[1] < 1)
						cmd[1] = 1;
				}
				else
				{
					cmd[2] = 0;
					cmd[3] = m_desired_acceleration;
					if (cmd[3] < 1)
						cmd[3] = 1;
				}

				m_driver.IOCtl(CMDVMove, (void *)cmd);
			}
			break;
		}
	}
}

void CCanControlDlg::OnUpdateParametersAddressofcards(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!m_driverok);
}

void CCanControlDlg::OnUpdateParametersNumberofcards(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!m_driverok);
}

void CCanControlDlg::OnButtonPrepare() 
{
	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		m_driver.IOCtl(CMDDisableAmp, &index);
		m_driver.IOCtl(CMDControllerIdle, &index);

		double tmp = 0;
		SingleAxisParameters par;
		par.parameters = (void *)&tmp;
		par.axis = index;

		m_driver.IOCtl(CMDDefinePosition, (void *)&par);
		m_driver.IOCtl(CMDSetCommand, (void *)&par);
		
		m_driver.IOCtl(CMDEnableAmp, &index);
		m_driver.IOCtl(CMDControllerRun, &index);
	}
}

void CCanControlDlg::OnButtonSetminmax() 
{
	UpdateData (TRUE);

	int index = m_axis_ctrl.GetCurSel();
	if (index != CB_ERR)
	{
		SingleAxisParameters x;
		x.axis = index;
		x.parameters = &m_min_position;
		m_driver.IOCtl(CMDSetPositiveLimit, (void *)&x);

		x.parameters = &m_max_position;
		m_driver.IOCtl(CMDSetNegativeLimit, (void *)&x);
	}
}

void CCanControlDlg::OnButtonSpy() 
{
	init_console();
}

void CCanControlDlg::OnFileOpenconsole() 
{
	init_console();
}

void CCanControlDlg::OnUpdateFileOpenconsole(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (m_driverok);
}

void CCanControlDlg::OnFileCloseconsole() 
{
	close_console();
}

void CCanControlDlg::OnUpdateFileCloseconsole(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (m_driverok);
}
