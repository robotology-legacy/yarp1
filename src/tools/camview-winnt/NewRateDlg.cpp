// NewRateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "camview.h"
#include "NewRateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewRateDlg dialog


CNewRateDlg::CNewRateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewRateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewRateDlg)
	m_period = 0;
	//}}AFX_DATA_INIT
}


void CNewRateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewRateDlg)
	DDX_Text(pDX, IDC_EDIT_NEWRATE, m_period);
	DDV_MinMaxInt(pDX, m_period, 0, 5000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewRateDlg, CDialog)
	//{{AFX_MSG_MAP(CNewRateDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewRateDlg message handlers
