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
	//{{AFX_DATA_INIT(CCardAddressDlg)
	memset (m_address, 0, sizeof(int) * CANBUS_MAXCARDS);
	//}}AFX_DATA_INIT
}


void CCardAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardAddressDlg)
	DDX_Text(pDX, IDC_EDIT_ADDRESS0, m_address[0]);
	DDV_MinMaxInt(pDX, m_address[0], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS1, m_address[1]);
	DDV_MinMaxInt(pDX, m_address[1], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS2, m_address[2]);
	DDV_MinMaxInt(pDX, m_address[2], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS3, m_address[3]);
	DDV_MinMaxInt(pDX, m_address[3], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS4, m_address[4]);
	DDV_MinMaxInt(pDX, m_address[4], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS5, m_address[5]);
	DDV_MinMaxInt(pDX, m_address[5], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS6, m_address[6]);
	DDV_MinMaxInt(pDX, m_address[6], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS7, m_address[7]);
	DDV_MinMaxInt(pDX, m_address[7], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS8, m_address[8]);
	DDV_MinMaxInt(pDX, m_address[8], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS9, m_address[9]);
	DDV_MinMaxInt(pDX, m_address[9], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS10, m_address[10]);
	DDV_MinMaxInt(pDX, m_address[10], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS11, m_address[11]);
	DDV_MinMaxInt(pDX, m_address[11], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS12, m_address[12]);
	DDV_MinMaxInt(pDX, m_address[12], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS13, m_address[13]);
	DDV_MinMaxInt(pDX, m_address[13], 0, 16);
	DDX_Text(pDX, IDC_EDIT_ADDRESS14, m_address[14]);
	DDV_MinMaxInt(pDX, m_address[14], 0, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardAddressDlg, CDialog)
	//{{AFX_MSG_MAP(CCardAddressDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardAddressDlg message handlers
