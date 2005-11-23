// CardAddressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CanControl.h"
#include "CardAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardAddressDlg dialog


CCardAddressDlg::CCardAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCardAddressDlg::IDD, pParent)
{
	memset (m_address, 0, sizeof(int) * CANBUS_MAXCARDS);
	memset (m_disabled, FALSE, sizeof(BOOL) * CANBUS_MAXCARDS);
	//{{AFX_DATA_INIT(CCardAddressDlg)
	//}}AFX_DATA_INIT
}


void CCardAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int i;
	for (i = 0; i < CANBUS_MAXCARDS; i++)
	{
		DDX_Text(pDX, IDC_EDIT_ADDRESS0+i, m_address[i]);
		DDV_MinMaxInt(pDX, m_address[i], 0, 255);
	}

	for (i = 0; i < CANBUS_MAXCARDS; i++)
		DDX_Check(pDX, IDC_CHECK1+i, m_disabled[i]);

	//{{AFX_DATA_MAP(CCardAddressDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardAddressDlg, CDialog)
	//{{AFX_MSG_MAP(CCardAddressDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardAddressDlg message handlers
