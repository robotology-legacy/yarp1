// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cameraTune.h"
#include "OptionsDlg.h"
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
	m_AutoSave = FALSE;
	m_FileName = _T("results.csv");
	m_BoardN = 0;
	m_SizeX = 640;
	m_SizeY = 480;
	m_Rho = 256;
	m_Theta = 180;
	m_RefreshTime = 100;
	m_BufferSize = 20;
	//}}AFX_DATA_INIT

}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Check(pDX, IDC_AUTOSAVE, m_AutoSave);
	DDX_Text(pDX, IDC_FILE_NAME, m_FileName);
	DDX_Text(pDX, IDC_BOARD_N, m_BoardN);
	DDX_Text(pDX, IDC_SIZE_X, m_SizeX);
	DDX_Text(pDX, IDC_SIZE_Y, m_SizeY);
	DDX_Text(pDX, IDC_RHO, m_Rho);
	DDX_Text(pDX, IDC_THETA, m_Theta);
	DDX_Text(pDX, IDC_REFRESH, m_RefreshTime);
	DDX_Text(pDX, N_IMAGES, m_BufferSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

void COptionsDlg::OnDefault() 
{
	m_AutoSave = FALSE;
	m_FileName = _T("results.csv");
	m_BoardN = 0;
	m_SizeX = 640;
	m_SizeY = 480;
	m_Rho = 256;
	m_Theta = 360;
	m_RefreshTime = 100;
	m_BufferSize = 20;
	UpdateData(FALSE);
}

void COptionsDlg::OnLoad() 
{
	YARPConfigFile optFile;	
	char buf[255];
	int dummy = 0;
	optFile.setName("cameraTune.ini");
	optFile.get("[CONFIG]", "AutoSave", &dummy);
	if (dummy != 0)
		m_AutoSave = TRUE;
	else 
		m_AutoSave = FALSE;
	if ( optFile.getString("[CONFIG]", "FileName", buf)  == YARP_OK)
		m_FileName = buf;
	optFile.get("[CONFIG]", "BoardNumber", &m_BoardN);
	optFile.get("[CONFIG]", "ImageWidth", &m_SizeX);
	optFile.get("[CONFIG]", "ImageHeight", &m_SizeY);
	optFile.get("[CONFIG]", "HoughRho", &m_Rho);
	optFile.get("[CONFIG]", "HoughTheta", &m_Theta);
	optFile.get("[CONFIG]", "RefreshTime", &m_RefreshTime);
	optFile.get("[CONFIG]", "BufferSize", &m_BufferSize);
	
	UpdateData(FALSE);
	
}

void COptionsDlg::OnSave() 
{
	FILE *optFile = NULL;
	optFile = fopen("cameraTune.ini","w");
	if (optFile == NULL)
	{
		MessageBox("Impossible to save to option file.", "Error.", MB_ICONERROR);
		return;
	}
	UpdateData(TRUE);
	fprintf(optFile,"[CONFIG]\n" );
	if (m_AutoSave == TRUE)
		fprintf(optFile,"AutoSave 1\n");
	else
		fprintf(optFile,"AutoSave 0\n");
	fprintf(optFile,"FileName %s\n", (LPCSTR)m_FileName);
	fprintf(optFile,"BoardNumber %d\n", m_BoardN);
	fprintf(optFile,"ImageWidth %d\n", m_SizeX);
	fprintf(optFile,"ImageHeight %d\n", m_SizeY);
	fprintf(optFile,"HoughRho %d\n", m_Rho);
	fprintf(optFile,"HoughTheta %d\n", m_Theta);
	fprintf(optFile,"RefreshTime %d\n", m_RefreshTime);
	fprintf(optFile,"BufferSize %d\n", m_BufferSize);

	fclose(optFile);
	
}
