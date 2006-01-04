// MessagesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GraspCapture.h"
#include "MessagesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
extern YARPInputPortOf<MNumData> _data_inport;
extern YARPInputPortOf<YARPGenericImage> _img_inport;
extern YARPInputPortOf<int> _rep_inport;
extern YARPOutputPortOf<MCommands> _cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
extern int _sizeX;
extern int _sizeY;
*/

/////////////////////////////////////////////////////////////////////////////
// CMessagesDlg dialog


CMessagesDlg::CMessagesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessagesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessagesDlg)
	//}}AFX_DATA_INIT
}


void CMessagesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessagesDlg)
	DDX_Control(pDX, IDC_MESS_BOX, m_MessageBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessagesDlg, CDialog)
	//{{AFX_MSG_MAP(CMessagesDlg)
	ON_EN_CHANGE(IDC_FREQUENCY, OnChangeFrequency)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessagesDlg message handlers

BOOL CMessagesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessagesDlg::OnChangeFrequency() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
