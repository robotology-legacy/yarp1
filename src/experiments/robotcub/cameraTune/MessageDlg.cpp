// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cameraTune.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog


CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDlg)
	initialOrientation = 0.0;
	finalOrientation = 0.0;
	m_Text = _T("");
	//}}AFX_DATA_INIT
}


void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	DDX_Text(pDX, IDC_OUTPUT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

void CMessageDlg::OnSave() 
{
	static TCHAR BASED_CODE szFilter[] = _T("Comma Separated Values Files (*.csv)|*.csv|Text Files (*.txt)|*.txt||");
	CFileDialog m_FileDlg(FALSE, NULL, NULL, NULL, szFilter);
	if (m_FileDlg.DoModal()== IDOK)
	{
		CString FileName = m_FileDlg.GetFileName();
		FILE *outFile;
		outFile = fopen((LPCSTR)FileName,"a");
		
		if (!outFile)
		{
			MessageBox("Unable to save to selected file.", "Error.",MB_ICONERROR);
		}
		else
			fprintf(outFile, "%f;%f\n", initialOrientation, finalOrientation);

		fclose(outFile);
		OnOK();
	}
	
}

void CMessageDlg::OnOK() 
{
	CDialog::OnOK();
}
