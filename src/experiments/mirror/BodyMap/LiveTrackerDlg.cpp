// LiveTrackerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BodyMap.h"
#include "LiveTrackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLiveTrackerDlg dialog


CLiveTrackerDlg::CLiveTrackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLiveTrackerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLiveTrackerDlg)
	m_Azimuth = 0.0f;
	m_Elevation = 0.0f;
	m_Roll = 0.0f;
	m_X = 0.0f;
	m_Y = 0.0f;
	m_Z = 0.0f;
	//}}AFX_DATA_INIT
}


void CLiveTrackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLiveTrackerDlg)
	DDX_Text(pDX, IDC_OR_AZIMUTH, m_Azimuth);
	DDX_Text(pDX, IDC_OR_ELEVATION, m_Elevation);
	DDX_Text(pDX, IDC_OR_ROLL, m_Roll);
	DDX_Text(pDX, IDC_POS_X, m_X);
	DDX_Text(pDX, IDC_POS_Y, m_Y);
	DDX_Text(pDX, IDC_POS_Z, m_Z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLiveTrackerDlg, CDialog)
	//{{AFX_MSG_MAP(CLiveTrackerDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLiveTrackerDlg message handlers

void CLiveTrackerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
}

void CLiveTrackerDlg::UpdateState(TrackerData newData)
{
	m_Azimuth = (float)newData.azimuth;
	m_Elevation = (float)newData.elevation;
	m_Roll = (float)newData.roll;
	m_X = (float)newData.x;
	m_Y = (float)newData.y;
	m_Z = (float)newData.z;
		
	UpdateData(FALSE);
}
