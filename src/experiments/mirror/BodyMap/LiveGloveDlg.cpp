// LiveGloveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BodyMap.h"
#include "LiveGloveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLiveGloveDlg dialog


CLiveGloveDlg::CLiveGloveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLiveGloveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLiveGloveDlg)
	m_Pres1 = 0;
	m_Pres2 = 0;
	m_Pres3 = 0;
	m_Pres4 = 0;
	m_abdTI = 0;
	m_abdIM = 0;
	m_abdMR = 0;
	m_abdRP = 0;
	m_finTI = 0;
	m_finTM = 0;
	m_finTO = 0;
	m_finII = 0;
	m_finIM = 0;
	m_finIO = 0;
	m_finMI = 0;
	m_finMM = 0;
	m_finMO = 0;
	m_finRI = 0;
	m_finRM = 0;
	m_finRO = 0;
	m_finPI = 0;
	m_finPM = 0;
	m_finPO = 0;
	m_palm = 0;
	m_wristABD = 0;
	m_wristFLX = 0;
	m_PupDiam = 0;
	m_pupX = 0;
	m_pupY = 0;
	//}}AFX_DATA_INIT
}


void CLiveGloveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLiveGloveDlg)
	DDX_Text(pDX, IDC_PRES_1, m_Pres1);
	DDX_Text(pDX, IDC_PRES_2, m_Pres2);
	DDX_Text(pDX, IDC_PRES_3, m_Pres3);
	DDX_Text(pDX, IDC_PRES_4, m_Pres4);
	DDX_Text(pDX, IDC_ABD_1, m_abdTI);
	DDX_Text(pDX, IDC_ABD_2, m_abdIM);
	DDX_Text(pDX, IDC_ABD_3, m_abdMR);
	DDX_Text(pDX, IDC_ABD_4, m_abdRP);
	DDX_Text(pDX, IDC_FINGER_1, m_finTI);
	DDX_Text(pDX, IDC_FINGER_2, m_finTM);
	DDX_Text(pDX, IDC_FINGER_3, m_finTO);
	DDX_Text(pDX, IDC_FINGER_4, m_finII);
	DDX_Text(pDX, IDC_FINGER_5, m_finIM);
	DDX_Text(pDX, IDC_FINGER_6, m_finIO);
	DDX_Text(pDX, IDC_FINGER_7, m_finMI);
	DDX_Text(pDX, IDC_FINGER_8, m_finMM);
	DDX_Text(pDX, IDC_FINGER_9, m_finMO);
	DDX_Text(pDX, IDC_FINGER_10, m_finRI);
	DDX_Text(pDX, IDC_FINGER_11, m_finRM);
	DDX_Text(pDX, IDC_FINGER_12, m_finRO);
	DDX_Text(pDX, IDC_FINGER_13, m_finPI);
	DDX_Text(pDX, IDC_FINGER_14, m_finPM);
	DDX_Text(pDX, IDC_FINGER_15, m_finPO);
	DDX_Text(pDX, IDC_PALM, m_palm);
	DDX_Text(pDX, IDC_WR_ABD, m_wristABD);
	DDX_Text(pDX, IDC_WR_FLX, m_wristFLX);
	DDX_Text(pDX, IDC_PUP_DIA, m_PupDiam);
	DDX_Text(pDX, IDC_PUP_X, m_pupX);
	DDX_Text(pDX, IDC_PUP_Y, m_pupY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLiveGloveDlg, CDialog)
	//{{AFX_MSG_MAP(CLiveGloveDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLiveGloveDlg message handlers

void CLiveGloveDlg::UpdateState(DataGloveData newGlove_d, PresSensData newPres_d, GazeTrackerData newGT_d)
{

	m_Pres1 = newPres_d.channelA;
	m_Pres2 = newPres_d.channelB;
	m_Pres3 = newPres_d.channelC;
	m_Pres4 = newPres_d.channelD;

	m_abdTI = newGlove_d.abduction[0];
	m_abdIM = newGlove_d.abduction[1];
	m_abdMR = newGlove_d.abduction[2];
	m_abdRP = newGlove_d.abduction[3];
	m_finTI = newGlove_d.thumb[0];
	m_finTM = newGlove_d.thumb[1];
	m_finTO = newGlove_d.thumb[2];
	m_finII = newGlove_d.index[0];
	m_finIM = newGlove_d.index[1];
	m_finIO = newGlove_d.index[2];
	m_finMI = newGlove_d.middle[0];
	m_finMM = newGlove_d.middle[1];
	m_finMO = newGlove_d.middle[2];
	m_finRI = newGlove_d.ring[0];
	m_finRM = newGlove_d.ring[1];
	m_finRO = newGlove_d.ring[2];
	m_finPI = newGlove_d.pinkie[0];
	m_finPM = newGlove_d.pinkie[1];
	m_finPO = newGlove_d.pinkie[2];
	m_palm = newGlove_d.palmArch;
	m_wristABD = newGlove_d.wrist[0];
	m_wristFLX = newGlove_d.wrist[1];

	m_PupDiam = newGT_d.pupilDiam;
	m_pupX = newGT_d.pupilX;
	m_pupY = newGT_d.pupilY;

	UpdateData(FALSE);

}
