// NetworkNumberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cancontrol.h"
#include "NetworkNumberDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkNumberDlg dialog


CNetworkNumberDlg::CNetworkNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetworkNumberDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetworkNumberDlg)
	m_busno = -1;
	//}}AFX_DATA_INIT
}


void CNetworkNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkNumberDlg)
	DDX_CBIndex(pDX, IDC_COMBO_BUSNUMBER, m_busno);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkNumberDlg, CDialog)
	//{{AFX_MSG_MAP(CNetworkNumberDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkNumberDlg message handlers
