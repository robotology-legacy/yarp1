// FingersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HandDemo.h"
#include "FingersDlg.h"

/*
#ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/


/////////////////////////////////////////////////////////////////////////////
// CFingersDlg dialog


CFingersDlg::CFingersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFingersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFingersDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFingersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFingersDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_POS1, m_pos[0]);
	DDX_Text(pDX, IDC_POS2, m_pos[1]);
	DDX_Text(pDX, IDC_POS3, m_pos[2]);
	DDX_Text(pDX, IDC_POS4, m_pos[3]);
	DDX_Text(pDX, IDC_POS5, m_pos[4]);
	DDX_Text(pDX, IDC_POS6, m_pos[5]);
	DDX_Text(pDX, IDC_POS7, m_pos[6]);
	DDX_Text(pDX, IDC_POS8, m_pos[7]);
	DDX_Text(pDX, IDC_POS9, m_pos[8]);
	DDX_Text(pDX, IDC_POS10, m_pos[9]);
	DDX_Text(pDX, IDC_POS11, m_pos[10]);
	DDX_Text(pDX, IDC_POS12, m_pos[11]);
	DDX_Text(pDX, IDC_POS13, m_pos[12]);
	DDX_Text(pDX, IDC_POS14, m_pos[13]);
	DDX_Text(pDX, IDC_POS15, m_pos[14]);
}


BEGIN_MESSAGE_MAP(CFingersDlg, CDialog)
	//{{AFX_MSG_MAP(CFingersDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFingersDlg message handlers
