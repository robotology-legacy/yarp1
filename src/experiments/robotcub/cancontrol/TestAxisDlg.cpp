// TestAxisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CanControl.h"
#include "TestAxisDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "CanControlParams.h"
#include "CanControlDlg.h"
#include "math.h"

/////////////////////////////////////////////////////////////////////////////
// CTestAxisDlg dialog


CTestAxisDlg::CTestAxisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestAxisDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestAxisDlg)
	m_repetitions = 1;
	m_startposition = 0.0;
	m_stopposition = 0.0;
	m_speed = 0.0;
	//}}AFX_DATA_INIT

	m_index = 0;
	m_repetition = 0;
}


void CTestAxisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestAxisDlg)
	DDX_Control(pDX, IDC_EDIT_VELOCITY, m_velocity_ctrl);
	DDX_Control(pDX, IDC_EDIT_STOP, m_stop_ctrl);
	DDX_Control(pDX, IDC_EDIT_START, m_start_ctrl);
	DDX_Control(pDX, IDC_EDIT_REPETITIONS, m_repetitions_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STOPM, m_stopm_ctrl);
	DDX_Control(pDX, IDC_BUTTON_STARTM, m_startm_ctrl);
	DDX_Text(pDX, IDC_EDIT_REPETITIONS, m_repetitions);
	DDV_MinMaxInt(pDX, m_repetitions, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_START, m_startposition);
	DDX_Text(pDX, IDC_EDIT_STOP, m_stopposition);
	DDX_Text(pDX, IDC_EDIT_VELOCITY, m_speed);
	DDV_MinMaxDouble(pDX, m_speed, 0., 32767.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestAxisDlg, CDialog)
	//{{AFX_MSG_MAP(CTestAxisDlg)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_STARTM, OnButtonStartm)
	ON_BN_CLICKED(IDC_BUTTON_STOPM, OnButtonStopm)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestAxisDlg message handlers

void CTestAxisDlg::OnButtonHide() 
{
	ShowWindow(SW_HIDE);	
}

void CTestAxisDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

void CTestAxisDlg::OnButtonStartm() 
{
	UpdateData (TRUE);

	CCanControlDlg& p = *(CCanControlDlg *)GetParent();

	m_index = p.m_axis_ctrl.GetCurSel();
	const int duration = (int)ceil (fabs(m_startposition - m_stopposition) / m_speed + .5);
	const int MARGIN = 200;

	if (m_index != CB_ERR && duration < 32767 && duration >= 1)
	{
		m_repetitions *= 2;
		m_repetition = 0;

		m_velocity_ctrl.EnableWindow (FALSE);
		m_start_ctrl.EnableWindow (FALSE);
		m_stop_ctrl.EnableWindow (FALSE);
		m_repetitions_ctrl.EnableWindow (FALSE);
		m_startm_ctrl.EnableWindow (FALSE);
		m_stopm_ctrl.EnableWindow (TRUE);

		/// go to start.
		SingleAxisParameters x;
		x.axis = m_index;				
		double param = 0;
		x.parameters = &param;

		if (m_speed < 1)
			m_speed = 1;

		param = m_speed;
		p.m_driver.IOCtl(CMDSetSpeed, (void *)&x);

		param = m_startposition;
		p.m_driver.IOCtl(CMDSetPosition, (void *)&x);
	
		SetTimer (TIMER2_ID, duration+MARGIN, NULL); 
	}
	else
	{
		MessageBox ("Can't generate trajectory with current parameters", "Error!");
	}
	
}

void CTestAxisDlg::OnButtonStopm() 
{
	m_velocity_ctrl.EnableWindow (TRUE);
	m_start_ctrl.EnableWindow (TRUE);
	m_stop_ctrl.EnableWindow (TRUE);
	m_repetitions_ctrl.EnableWindow (TRUE);
	m_startm_ctrl.EnableWindow (TRUE);
	m_stopm_ctrl.EnableWindow (FALSE);

	KillTimer (TIMER2_ID);
}

void CTestAxisDlg::OnTimer(UINT nIDEvent) 
{
	if (m_repetition < m_repetitions)
	{
		m_repetition ++;
		if ((m_repetition % 2) == 0)
		{
			CCanControlDlg& p = *(CCanControlDlg *)GetParent();

			/// go to start.
			SingleAxisParameters x;
			x.axis = m_index;				
			double param = 0;
			x.parameters = &param;

			if (m_speed < 1)
				m_speed = 1;

			param = m_speed;
			p.m_driver.IOCtl(CMDSetSpeed, (void *)&x);

			param = m_startposition;
			p.m_driver.IOCtl(CMDSetPosition, (void *)&x);
		}
		else
		{
			/// go to stop.
			CCanControlDlg& p = *(CCanControlDlg *)GetParent();

			/// go to start.
			SingleAxisParameters x;
			x.axis = m_index;				
			double param = 0;
			x.parameters = &param;

			if (m_speed < 1)
				m_speed = 1;

			param = m_speed;
			p.m_driver.IOCtl(CMDSetSpeed, (void *)&x);

			param = m_stopposition;
			p.m_driver.IOCtl(CMDSetPosition, (void *)&x);
		}

		CDialog::OnTimer(nIDEvent);
	}
	else
	{
		m_velocity_ctrl.EnableWindow (TRUE);
		m_start_ctrl.EnableWindow (TRUE);
		m_stop_ctrl.EnableWindow (TRUE);
		m_repetitions_ctrl.EnableWindow (TRUE);
		m_startm_ctrl.EnableWindow (TRUE);
		m_stopm_ctrl.EnableWindow (FALSE);

		KillTimer (TIMER2_ID);
	}
}
