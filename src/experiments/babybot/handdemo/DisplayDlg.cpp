// DisplayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HandDemo.h"
#include "DisplayDlg.h"

/*
#ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/


/////////////////////////////////////////////////////////////////////////////
// CDisplayDlg dialog


CDisplayDlg::CDisplayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisplayDlg)
	m_inmotion1 = FALSE;
	m_inmotion2 = FALSE;
	m_inmotion3 = FALSE;
	m_inmotion4 = FALSE;
	m_inmotion5 = FALSE;
	m_inmotion6 = FALSE;
	//}}AFX_DATA_INIT

	memset(m_pos, 0, sizeof(m_pos));
	memset(m_sp, 0, sizeof(m_sp));
}


void CDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayDlg)
	DDX_Text(pDX, IDC_DPOS_1, m_pos[0]);
	DDX_Text(pDX, IDC_DPOS_2, m_pos[1]);
	DDX_Text(pDX, IDC_DPOS_3, m_pos[2]);
	DDX_Text(pDX, IDC_DPOS_4, m_pos[3]);
	DDX_Text(pDX, IDC_DPOS_5, m_pos[4]);
	DDX_Text(pDX, IDC_DPOS_6, m_pos[5]);
	DDX_Check(pDX, IDC_MOVING1, m_inmotion1);
	DDX_Check(pDX, IDC_MOVING2, m_inmotion2);
	DDX_Check(pDX, IDC_MOVING3, m_inmotion3);
	DDX_Check(pDX, IDC_MOVING4, m_inmotion4);
	DDX_Check(pDX, IDC_MOVING5, m_inmotion5);
	DDX_Check(pDX, IDC_MOVING6, m_inmotion6);
	DDX_Text(pDX, IDC_DSP_1, m_sp[0]);
	DDX_Text(pDX, IDC_DSP_2, m_sp[1]);
	DDX_Text(pDX, IDC_DSP_3, m_sp[2]);
	DDX_Text(pDX, IDC_DSP_4, m_sp[3]);
	DDX_Text(pDX, IDC_DSP_5, m_sp[4]);
	DDX_Text(pDX, IDC_DSP_6, m_sp[5]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayDlg, CDialog)
	//{{AFX_MSG_MAP(CDisplayDlg)
	ON_BN_CLICKED(IDCANCEL, OnHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayDlg message handlers

void CDisplayDlg::OnHide() 
{
	// TODO: Add your control notification handler code here
	
}
