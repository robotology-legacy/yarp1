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
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalibrationDlg)
	DDX_Control(pDX, IDC_BUTTON_ROLLTILT, m_rolltilt_ctrl);
	DDX_Control(pDX, IDC_BUTTON_NECKPAN, m_neckpan_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J4, m_j4_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J3, m_j3_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J2, m_j2_ctrl);
	DDX_Control(pDX, IDC_BUTTON_J1, m_j1_ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalibrationDlg, CDialog)
	//{{AFX_MSG_MAP(CCalibrationDlg)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_BN_CLICKED(IDC_BUTTON_J1, OnButtonJ1)
	ON_BN_CLICKED(IDC_BUTTON_J2, OnButtonJ2)
	ON_BN_CLICKED(IDC_BUTTON_J3, OnButtonJ3)
	ON_BN_CLICKED(IDC_BUTTON_J4, OnButtonJ4)
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
}

void CCalibrationDlg::DisableInterface()
{
	m_j1_ctrl.EnableWindow(FALSE);
	m_j2_ctrl.EnableWindow(FALSE);
	m_j3_ctrl.EnableWindow(FALSE);
	m_j4_ctrl.EnableWindow(FALSE);
	m_neckpan_ctrl.EnableWindow(FALSE);
	m_rolltilt_ctrl.EnableWindow(FALSE);
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
