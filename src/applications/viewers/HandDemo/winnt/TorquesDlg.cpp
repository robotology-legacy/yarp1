// TorquesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HandDemo.h"
#include "TorquesDlg.h"

/*
#ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/

/////////////////////////////////////////////////////////////////////////////
// CTorquesDlg dialog


CTorquesDlg::CTorquesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTorquesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTorquesDlg)
	//}}AFX_DATA_INIT

	memset(m_t, 0, sizeof(m_t));
}


void CTorquesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTorquesDlg)
	DDX_Text(pDX, IDC_TORQUE_1, m_t[0]);
	DDX_Text(pDX, IDC_TORQUE_2, m_t[1]);
	DDX_Text(pDX, IDC_TORQUE_3, m_t[2]);
	DDX_Text(pDX, IDC_TORQUE_4, m_t[3]);
	DDX_Text(pDX, IDC_TORQUE_5, m_t[4]);
	DDX_Text(pDX, IDC_TORQUE_6, m_t[5]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTorquesDlg, CDialog)
	//{{AFX_MSG_MAP(CTorquesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTorquesDlg message handlers
