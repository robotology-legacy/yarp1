// SetAddressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CanControl.h"
#include "SetAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetAddressDlg dialog


CSetAddressDlg::CSetAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetAddressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetAddressDlg)
	m_address = 0;
	//}}AFX_DATA_INIT
}


void CSetAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetAddressDlg)
	DDX_Text(pDX, IDC_ADDRESS_NO, m_address);
	DDV_MinMaxInt(pDX, m_address, 0, 15);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetAddressDlg, CDialog)
	//{{AFX_MSG_MAP(CSetAddressDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetAddressDlg message handlers
