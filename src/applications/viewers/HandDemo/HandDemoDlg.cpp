// HandDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HandDemo.h"
#include "HandDemoDlg.h"

/*
#ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

/////////////////////////////////////////////////////////////////////////////
// CHandDemoDlg dialog

CHandDemoDlg::CHandDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHandDemoDlg::IDD, pParent),
	_hand_thread(__hand_rate)
{
	//{{AFX_DATA_INIT(CHandDemoDlg)
	m_pos1 = 0;
	m_pos2 = 0;
	m_pos3 = 0;
	m_pos4 = 0;
	m_pos5 = 0;
	m_pos6 = 0;
	m_gain_prop1 = 0.0;
	m_gain_prop2 = 0.0;
	m_gain_prop3 = 0.0;
	m_gain_prop4 = 0.0;
	m_gain_prop5 = 0.0;
	m_gain_prop6 = 0.0;
	m_acc1 = 0;
	m_acc2 = 0;
	m_acc3 = 0;
	m_acc4 = 0;
	m_acc5 = 0;
	m_acc6 = 0;
	m_sp1 = 0;
	m_sp2 = 0;
	m_sp3 = 0;
	m_sp5 = 0;
	m_sp4 = 0;
	m_sp6 = 0;
	m_gain_der1 = 0.0;
	m_gain_der2 = 0.0;
	m_gain_der3 = 0.0;
	m_gain_der4 = 0.0;
	m_gain_der5 = 0.0;
	m_gain_der6 = 0.0;
	m_gain_int1 = 0.0;
	m_gain_int2 = 0.0;
	m_gain_int3 = 0.0;
	m_gain_int4 = 0.0;
	m_gain_int5 = 0.0;
	m_gain_int6 = 0.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHandDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHandDemoDlg)
	DDX_Text(pDX, IDC_POS_1, m_pos1);
	DDX_Text(pDX, IDC_POS_2, m_pos2);
	DDX_Text(pDX, IDC_POS_3, m_pos3);
	DDX_Text(pDX, IDC_POS_4, m_pos4);
	DDX_Text(pDX, IDC_POS_5, m_pos5);
	DDX_Text(pDX, IDC_POS_6, m_pos6);
	DDX_Text(pDX, IDC_GAIN_PROP1, m_gain_prop1);
	DDX_Text(pDX, IDC_GAIN_PROP2, m_gain_prop2);
	DDX_Text(pDX, IDC_GAIN_PROP3, m_gain_prop3);
	DDX_Text(pDX, IDC_GAIN_PROP4, m_gain_prop4);
	DDX_Text(pDX, IDC_GAIN_PROP5, m_gain_prop5);
	DDX_Text(pDX, IDC_GAIN_PROP6, m_gain_prop6);
	DDX_Text(pDX, IDC_AC_1, m_acc1);
	DDX_Text(pDX, IDC_AC_2, m_acc2);
	DDX_Text(pDX, IDC_AC_3, m_acc3);
	DDX_Text(pDX, IDC_AC_4, m_acc4);
	DDX_Text(pDX, IDC_AC_5, m_acc5);
	DDX_Text(pDX, IDC_AC_6, m_acc6);
	DDX_Text(pDX, IDC_SP_1, m_sp1);
	DDX_Text(pDX, IDC_SP_2, m_sp2);
	DDX_Text(pDX, IDC_SP_3, m_sp3);
	DDX_Text(pDX, IDC_SP_5, m_sp5);
	DDX_Text(pDX, IDC_SP_4, m_sp4);
	DDX_Text(pDX, IDC_SP_6, m_sp6);
	DDX_Text(pDX, IDC_GAIN_DER1, m_gain_der1);
	DDX_Text(pDX, IDC_GAIN_DER2, m_gain_der2);
	DDX_Text(pDX, IDC_GAIN_DER3, m_gain_der3);
	DDX_Text(pDX, IDC_GAIN_DER4, m_gain_der4);
	DDX_Text(pDX, IDC_GAIN_DER5, m_gain_der5);
	DDX_Text(pDX, IDC_GAIN_DER6, m_gain_der6);
	DDX_Text(pDX, IDC_GAIN_INT1, m_gain_int1);
	DDX_Text(pDX, IDC_GAIN_INT2, m_gain_int2);
	DDX_Text(pDX, IDC_GAIN_INT3, m_gain_int3);
	DDX_Text(pDX, IDC_GAIN_INT4, m_gain_int4);
	DDX_Text(pDX, IDC_GAIN_INT5, m_gain_int5);
	DDX_Text(pDX, IDC_GAIN_INT6, m_gain_int6);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHandDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CHandDemoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDGO, OnGo)
	ON_BN_CLICKED(IDRESET, OnReset)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDSTOP, OnStop)
	ON_BN_CLICKED(IDUPDATEGAINS, OnUpdategains)
	ON_BN_CLICKED(IDUPDATESPEEDS, OnUpdatespeeds)
	ON_BN_CLICKED(IDVMOVE, OnVmove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHandDemoDlg message handlers

BOOL CHandDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	_hand_thread.start();
	AfxMessageBox("You can now turn on the hand...");
	_hand_thread.synchro();

	m_gain_prop1 = 0;
	m_gain_prop2 = 0;
	m_gain_prop3 = 0;
	m_gain_prop4 = 0;
	m_gain_prop5 = 0;
	m_gain_prop6 = 0;

	m_pos1 = 0;
	m_pos2 = 0;
	m_pos3 = 0;
	m_pos4 = 0;
	m_pos5 = 0;
	m_pos6 = 0;

	// update speeds and accelerations
	double sp[6];
	double ac[6];

	_hand_thread._hand.getRefSpeedsRaw(sp);
	_hand_thread._hand.getRefAccelerationsRaw(ac);

	m_sp1 = sp[0];
	m_sp2 = sp[1];
	m_sp3 = sp[2];
	m_sp4 = sp[3];
	m_sp5 = sp[4];
	m_sp6 = sp[5];
	
	m_acc1 = ac[0];
	m_acc2 = ac[1];
	m_acc3 = ac[2];
	m_acc4 = ac[3];
	m_acc5 = ac[4];
	m_acc6 = ac[5];

	// update pids
	LowLevelPID pid;
	_hand_thread._hand.getPID(pid, 1);
	m_gain_prop1 = pid.KP;
	m_gain_der1 = pid.KD;
	m_gain_int1 = pid.KI;

	_hand_thread._hand.getPID(pid, 2);
	m_gain_prop2 = pid.KP;
	m_gain_der2 = pid.KD;
	m_gain_int2 = pid.KI;

	_hand_thread._hand.getPID(pid, 3);

	m_gain_prop3 = pid.KP;
	m_gain_der3= pid.KD;
	m_gain_int3 = pid.KI;

	_hand_thread._hand.getPID(pid, 4);
	m_gain_prop4 = pid.KP;
	m_gain_der4 = pid.KD;
	m_gain_int4 = pid.KI;

	_hand_thread._hand.getPID(pid, 5);
	m_gain_prop5 = pid.KP;
	m_gain_der5 = pid.KD;
	m_gain_int5= pid.KI;

	_hand_thread._hand.getPID(pid, 6);
	m_gain_prop6 = pid.KP;
	m_gain_der6 = pid.KD;
	m_gain_int6 = pid.KI;

	UpdateData(FALSE);

	// start timer
	_timer = SetTimer(666, 100, NULL);
	
	// create joint display dlg
	_display_dlg.Create(IDD_JOINTS, this);
	_display_dlg.ShowWindow(SW_SHOW);

	// create torques dlg
	_torques_dlg.Create(IDD_TORQUES, this);
	_torques_dlg.ShowWindow(SW_SHOW);

	// create finger display dlg
	_fingers_dlg.Create(IDD_FINGERSDLG, this);
	_fingers_dlg.ShowWindow(SW_SHOW);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHandDemoDlg::OnPaint() 
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
HCURSOR CHandDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHandDemoDlg::OnCancel() 
{
	KillTimer(_timer);

	_hand_thread.terminate();
		
	CDialog::OnCancel();
}

void CHandDemoDlg::OnGo() 
{
	UpdateData(TRUE);

	command[0] = m_pos1;
	command[1] = m_pos2;
	command[2] = m_pos3;
	command[3] = m_pos4;
	command[4] = m_pos5;
	command[5] = m_pos6;

	_hand_thread._hand.setPositionsRaw(command);

}

void CHandDemoDlg::OnOK() 
{
	KillTimer(_timer);

	_hand_thread.terminate();

	CDialog::OnOK();
}

void CHandDemoDlg::OnReset() 
{
	_hand_thread._hand.resetEncoders();
}

void CHandDemoDlg::OnTimer(UINT nIDEvent) 
{
	double dummy[6];
	char motion[6];
	
	// position
	_hand_thread._hand.getPositionsRaw(dummy);
	for(int i = 0; i < 6; i++)
		_display_dlg.m_pos[i] = dummy[i];

	// speeds
	_hand_thread._hand.getSpeedsRaw(dummy);
	for(i = 0; i < 6; i++)
		_display_dlg.m_sp[i] = dummy[i];

	// flag
	_hand_thread._hand.isMoving(motion);
	_display_dlg.m_inmotion1 = (motion[0] != 0) ? 1:0;
	_display_dlg.m_inmotion2 = (motion[1] != 0) ? 1:0;
	_display_dlg.m_inmotion3 = (motion[2] != 0) ? 1:0;
	_display_dlg.m_inmotion4 = (motion[3] != 0) ? 1:0;
	_display_dlg.m_inmotion5 = (motion[4] != 0) ? 1:0;
	_display_dlg.m_inmotion6 = (motion[5] != 0) ? 1:0;

	// torques
	_hand_thread._hand.getMotorTorques(_torques_dlg.m_t);
	
	// finger position -- hall sensors
	double h[15];
	_hand_thread._hand.getHallRaw(h);
	for(i = 0; i < 15; i++)
		_fingers_dlg.m_pos[i].Format("%.2lf", h[i]);

	_hand_thread._hand.getRefPositions(_ref_pos);
	_hand_thread._hand.getMotorErrors(_errors);

	unsigned char tmp;
	_hand_thread._hand.getInput(&tmp);

	_display_dlg.m_input = (int) tmp;

	_display_dlg.UpdateData(FALSE);
	_torques_dlg.UpdateData(FALSE);
	_fingers_dlg.UpdateData(FALSE);
	
	CDialog::OnTimer(nIDEvent);
}

void CHandDemoDlg::OnStop() 
{
	_hand_thread._hand.stopAxes();	
}

void CHandDemoDlg::OnUpdategains() 
{
	UpdateData(TRUE);

	LowLevelPID pid;
	pid.KP = m_gain_prop1;
	pid.KD = m_gain_der1;
	pid.KI = m_gain_int1;
	_hand_thread._hand.setPid(1, pid);
	
	pid.KP = m_gain_prop2;
	pid.KD = m_gain_der2;
	pid.KI = m_gain_int2;
	_hand_thread._hand.setPid(2, pid);

	pid.KP = m_gain_prop3;
	pid.KD = m_gain_der3;
	pid.KI = m_gain_int3;
	_hand_thread._hand.setPid(3, pid);

	pid.KP = m_gain_prop4;
	pid.KD = m_gain_der4;
	pid.KI = m_gain_int4;
	_hand_thread._hand.setPid(4, pid);

	pid.KP = m_gain_prop5;
	pid.KD = m_gain_der5;
	pid.KI = m_gain_int5;
	_hand_thread._hand.setPid(5, pid);

	pid.KP = m_gain_prop6;
	pid.KD = m_gain_der6;
	pid.KI = m_gain_int6;
	_hand_thread._hand.setPid(6, pid);
}

void CHandDemoDlg::OnUpdatespeeds() 
{
	UpdateData(TRUE);

	command[0] = m_sp1;
	command[1] = m_sp2;
	command[2] = m_sp3;
	command[3] = m_sp4;
	command[4] = m_sp5;
	command[5] = m_sp6;

	_hand_thread._hand.setSpeedsRaw(command);

	command[0] = m_acc1;
	command[1] = m_acc2;
	command[2] = m_acc3;
	command[3] = m_acc4;
	command[4] = m_acc5;
	command[5] = m_acc6;

	_hand_thread._hand.setAccelerationsRaw(command);
	
}

void CHandDemoDlg::OnVmove() 
{
	UpdateData(TRUE);

	command[0] = m_acc1;
	command[1] = m_acc2;
	command[2] = m_acc3;
	command[3] = m_acc4;
	command[4] = m_acc5;
	command[5] = m_acc6;

	_hand_thread._hand.setAccelerationsRaw(command);

	command[0] = m_sp1;
	command[1] = m_sp2;
	command[2] = m_sp3;
	command[3] = m_sp4;
	command[4] = m_sp5;
	command[5] = m_sp6;

	_hand_thread._hand.velocityMoves(command);


}
