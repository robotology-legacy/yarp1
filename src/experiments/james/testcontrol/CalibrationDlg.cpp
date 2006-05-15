// CalibrationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testcontrol.h"
#include "CalibrationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern YARPHead head;
extern YARPArm arm;

/////////////////////////////////////////////////////////////////////////////
// CCalibrationDlg dialog


CCalibrationDlg::CCalibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalibrationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalibrationDlg)
	//}}AFX_DATA_INIT
}


void CCalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
    int i;
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalibrationDlg)
	DDX_Control(pDX, IDC_BUTTON_A_CAL, m_button_acal);
	DDX_Control(pDX, IDC_BUTTON_H_CAL, m_button_hcal);
	DDX_Control(pDX, IDC_BUTTON_ROLLTILT, m_rolltilt_ctrl);
	DDX_Control(pDX, IDC_BUTTON_NECKPAN, m_neckpan_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J4, m_j4_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J3, m_j3_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J2, m_j2_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J1, m_j1_ctrl);
	//}}AFX_DATA_MAP
    for (i = 0; i < MAX_ARM_JNTS; i++)
	    DDX_Control(pDX, IDC_BUTTON_A_J0+i, m_button_aj[i]);
}


BEGIN_MESSAGE_MAP(CCalibrationDlg, CDialog)
	//{{AFX_MSG_MAP(CCalibrationDlg)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_BN_CLICKED(IDC_BUTTON_J1, OnButtonJ1)
	ON_BN_CLICKED(IDC_BUTTON_J2, OnButtonJ2)
	ON_BN_CLICKED(IDC_BUTTON_J3, OnButtonJ3)
	ON_BN_CLICKED(IDC_BUTTON_J4, OnButtonJ4)
	ON_BN_CLICKED(IDC_BUTTON_A_J0, OnButtonAJ0)
	ON_BN_CLICKED(IDC_BUTTON_A_J1, OnButtonAJ1)
	ON_BN_CLICKED(IDC_BUTTON_A_J2, OnButtonAJ2)
	ON_BN_CLICKED(IDC_BUTTON_A_J3, OnButtonAJ3)
	ON_BN_CLICKED(IDC_BUTTON_A_J4, OnButtonAJ4)
	ON_BN_CLICKED(IDC_BUTTON_A_J5, OnButtonAJ5)
	ON_BN_CLICKED(IDC_BUTTON_A_J6, OnButtonAJ6)
	ON_BN_CLICKED(IDC_BUTTON_A_J7, OnButtonAJ7)
	ON_BN_CLICKED(IDC_BUTTON_A_J8, OnButtonAJ8)
	ON_BN_CLICKED(IDC_BUTTON_A_J9, OnButtonAJ9)
	ON_BN_CLICKED(IDC_BUTTON_A_J10, OnButtonAJ10)
	ON_BN_CLICKED(IDC_BUTTON_A_J11, OnButtonAJ11)
	ON_BN_CLICKED(IDC_BUTTON_A_J12, OnButtonAJ12)
	ON_BN_CLICKED(IDC_BUTTON_A_J13, OnButtonAJ13)
	ON_BN_CLICKED(IDC_BUTTON_A_J14, OnButtonAJ14)
	ON_BN_CLICKED(IDC_BUTTON_A_CAL, OnButtonACal)
	ON_BN_CLICKED(IDC_BUTTON_H_CAL, OnButtonHCal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalibrationDlg message handlers

void CCalibrationDlg::OnButtonHide() 
{
	ShowWindow (SW_HIDE);	
}

void CCalibrationDlg::EnableInterface()
{
	m_j1_ctrl.EnableWindow();
	m_j2_ctrl.EnableWindow();
	m_j3_ctrl.EnableWindow();
	m_j4_ctrl.EnableWindow();
	m_neckpan_ctrl.EnableWindow();
	m_rolltilt_ctrl.EnableWindow();

    int i;
    for (i = 0; i < MAX_ARM_JNTS; i++)
        m_button_aj[i].EnableWindow();

	m_button_acal.EnableWindow();
	m_button_hcal.EnableWindow();
}

void CCalibrationDlg::DisableInterface()
{
	m_j1_ctrl.EnableWindow(FALSE);
	m_j2_ctrl.EnableWindow(FALSE);
	m_j3_ctrl.EnableWindow(FALSE);
	m_j4_ctrl.EnableWindow(FALSE);
	m_neckpan_ctrl.EnableWindow(FALSE);
	m_rolltilt_ctrl.EnableWindow(FALSE);

    int i;
    for (i = 0; i < MAX_ARM_JNTS; i++)
        m_button_aj[i].EnableWindow(FALSE);

	m_button_acal.EnableWindow(FALSE);
	m_button_hcal.EnableWindow(FALSE);
}

void CCalibrationDlg::OnButtonJ1() 
{
	head.calibrate (0);
}

void CCalibrationDlg::OnButtonJ2() 
{
	head.calibrate (2);
}

void CCalibrationDlg::OnButtonJ3() 
{
	head.calibrate (1);
}

void CCalibrationDlg::OnButtonJ4() 
{
	head.calibrate (3);
}

void CCalibrationDlg::OnButtonAJ0() 
{
    arm.calibrate (0);
}

void CCalibrationDlg::OnButtonAJ1() 
{
    arm.calibrate (1);
}

void CCalibrationDlg::OnButtonAJ2() 
{
    arm.calibrate (2);
}

void CCalibrationDlg::OnButtonAJ3() 
{
    arm.calibrate (3);
}

void CCalibrationDlg::OnButtonAJ4() 
{
    arm.calibrate (4);
}

void CCalibrationDlg::OnButtonAJ5() 
{
    arm.calibrate (5);
}

void CCalibrationDlg::OnButtonAJ6() 
{
    arm.calibrate (6);
}

void CCalibrationDlg::OnButtonAJ7() 
{
    arm.calibrate (7);
}

void CCalibrationDlg::OnButtonAJ8() 
{
    arm.calibrate (8);
}

void CCalibrationDlg::OnButtonAJ9() 
{
    arm.calibrate (9);
}

void CCalibrationDlg::OnButtonAJ10() 
{
    arm.calibrate (10);
}

void CCalibrationDlg::OnButtonAJ11() 
{
    arm.calibrate (11);
}

void CCalibrationDlg::OnButtonAJ12() 
{
    arm.calibrate (12);
}

void CCalibrationDlg::OnButtonAJ13() 
{
    arm.calibrate (13);
}

void CCalibrationDlg::OnButtonAJ14() 
{
    arm.calibrate (14);
}

void CCalibrationDlg::OnButtonACal() 
{
    arm.calibrate (1);
    arm.calibrate (0);
    int i;
    for (i = 2; i < MAX_ARM_JNTS; i++)
        arm.calibrate (i);
}

void CCalibrationDlg::OnButtonHCal() 
{
}
