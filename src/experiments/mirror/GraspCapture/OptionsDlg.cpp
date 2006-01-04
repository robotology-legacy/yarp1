// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "graspcapture.h"
#include "OptionsDlg.h"
#include "PathDialog.h"
#include <yarp/YARPConfigFile.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_NetName = _T("default");
	m_PortName = _T("mirrorGrasp");
	m_Prefix = _T("seq");
	m_RefreshTime = 40;
	m_SavePath = _T("d:\\tmp");
	//}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Text(pDX, IDC_NET_N, m_NetName);
	DDX_Text(pDX, IDC_PORT_N, m_PortName);
	DDX_Text(pDX, IDC_PREFIX, m_Prefix);
	DDX_Text(pDX, IDC_REFRESH, m_RefreshTime);
	DDX_Text(pDX, IDC_SAVEPATH, m_SavePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_SETPATH, OnSetPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

void COptionsDlg::OnDefault() 
{

	m_PortName = "mirrorGrasp";
	m_NetName = "default";
	m_SavePath = "d:\\tmp";
	m_Prefix = "seq";
	m_RefreshTime = 40;
	UpdateData(FALSE);
}

void COptionsDlg::OnLoad() 
{

	YARPConfigFile optFile;	

	optFile.setName("GraspCapture.ini");
	char buf[255];
	if ( optFile.getString("[CONFIG]", "NetName", buf )  == YARP_OK)
		m_NetName = buf;
	if ( optFile.getString("[CONFIG]", "PortName", buf)  == YARP_OK)
		m_PortName = buf;
	if ( optFile.getString("[CONFIG]", "Prefix", buf)  == YARP_OK)
		m_Prefix = buf;
	optFile.get("[CONFIG]", "Refresh", &m_RefreshTime);
	if ( optFile.getString("[CONFIG]", "SavePath", buf)  == YARP_OK)
		m_SavePath = buf;

	UpdateData(FALSE);
}

void COptionsDlg::OnSave() 
{
	FILE *optFile = NULL;
	optFile = fopen("GraspCapture.ini","w");
	if (optFile == NULL)
	{
		MessageBox("Impossible to save to option file.", "Error.", MB_ICONERROR);
		return;
	}
	UpdateData(TRUE);
	fprintf(optFile,"[CONFIG]\n" );
	fprintf(optFile,"NetName %s\n", (LPCSTR)m_NetName);
	fprintf(optFile,"PortName %s\n", (LPCSTR)m_PortName);
	fprintf(optFile,"Prefix %s\n", (LPCSTR)m_Prefix);
	fprintf(optFile,"SavePath %s\n", (LPCSTR)m_SavePath);
	fprintf(optFile,"Refresh %d\n", (LPCSTR)m_RefreshTime);
	
	fclose(optFile);
}		

void COptionsDlg::OnSetPath() 
{
	CString strPath;
	CString strCaption("Select ouput directory");
	CString strTitle("Please enter the output directory:");
	CPathDialog dlg(strCaption, strTitle, strPath);


	if(dlg.DoModal()==IDOK)
	{
		m_SavePath = dlg.GetPathName();
		UpdateData(FALSE);

	}	
}
