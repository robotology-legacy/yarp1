// CardsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CanControl.h"
#include "CardsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardsDlg dialog


CCardsDlg::CCardsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCardsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCardsDlg)
	m_cards = 0;
	//}}AFX_DATA_INIT
}


void CCardsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsDlg)
	DDX_Text(pDX, IDC_EDIT1, m_cards);
	DDV_MinMaxInt(pDX, m_cards, 1, 15);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsDlg, CDialog)
	//{{AFX_MSG_MAP(CCardsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsDlg message handlers
