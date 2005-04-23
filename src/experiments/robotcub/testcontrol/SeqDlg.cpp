// SeqDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testcontrol.h"
#include "testcontroldlg.h"
#include "SeqDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern YARPHead head;
extern YARPArm arm;

// CAN bus numbers.
#define HEAD 0
#define ARM 1


/////////////////////////////////////////////////////////////////////////////
// CSeqDlg dialog


CSeqDlg::CSeqDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSeqDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSeqDlg)
	//}}AFX_DATA_INIT

	int i;
	for (i = 0; i < SEQUENCE_LEN; i++)
	{
		m_s[i] = -1;
		m_delay[i] = 0;
	}
}


void CSeqDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSeqDlg)
	//}}AFX_DATA_MAP

	int i;
	// must be consecutive in resource.h
	for (i = 0; i < SEQUENCE_LEN; i++)
	{
		DDX_Text(pDX, IDC_EDIT1+i, m_s[i]);
		DDV_MinMaxInt(pDX, m_s[i], -1, N_POSTURES-1);
		DDX_Text(pDX, IDC_WAIT_1+i, m_delay[i]);
		DDV_MinMaxInt(pDX, m_delay[i], 0, 12000);
	}
}


BEGIN_MESSAGE_MAP(CSeqDlg, CDialog)
	//{{AFX_MSG_MAP(CSeqDlg)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_LOOP, OnButtonLoop)
	ON_BN_CLICKED(IDC_BUTTON_STOPL, OnButtonStopl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSeqDlg::EnableInterface (void)
{
	HWND w;
	int i;
	// must be consecutive in resource.h
	for (i = 0; i < SEQUENCE_LEN; i++)
	{
		GetDlgItem(IDC_EDIT1+i, &w);
		::EnableWindow (w, TRUE);
		GetDlgItem(IDC_WAIT_1+i, &w);
		::EnableWindow (w, TRUE);
	}


	GetDlgItem(IDC_BUTTON_RUN, &w);
	::EnableWindow (w, TRUE);

	GetDlgItem(IDC_BUTTON_LOOP, &w);
	::EnableWindow (w, TRUE);	

	GetDlgItem(IDC_BUTTON_STOPL, &w);
	::EnableWindow (w, TRUE);	
}

void CSeqDlg::DisableInterface (void)
{
	HWND w;
	int i;
	// must be consecutive in resource.h
	for (i = 0; i < SEQUENCE_LEN; i++)
	{
		GetDlgItem(IDC_EDIT1+i, &w);
		::EnableWindow (w, FALSE);
		GetDlgItem(IDC_WAIT_1+i, &w);
		::EnableWindow (w, FALSE);
	}

	GetDlgItem(IDC_BUTTON_RUN, &w);
	::EnableWindow (w, FALSE);

	GetDlgItem(IDC_BUTTON_LOOP, &w);
	::EnableWindow (w, FALSE);	

	GetDlgItem(IDC_BUTTON_STOPL, &w);
	::EnableWindow (w, FALSE);	
}

/////////////////////////////////////////////////////////////////////////////
// CSeqDlg message handlers

void CSeqDlg::OnButtonHide() 
{
	ShowWindow (SW_HIDE);	
}

void CSeqDlg::OnButtonRun() 
{
	UpdateData (TRUE);
	CTestControlDlg& p = *(CTestControlDlg *)GetParent();
	
	int i;
	for (i = 0; i < SEQUENCE_LEN; i++)
	{
		if (m_s[i] < 0 || m_s[i] >= N_POSTURES)
			break;

		if (p._headrunning && p._armrunning)
		{
			bool finished = false;
			head.setVelocities (p._headstorev[m_s[i]]);
			arm.setVelocities (p._armstorev[m_s[i]]);
			head.setPositions (p._headstore[m_s[i]]);
			arm.setPositions (p._armstore[m_s[i]]);

			YARPTime::DelayInSeconds (0.2);

			// wait.
			int timeout = 0;
			while (!finished)
			{
				finished = head.checkMotionDone();
				finished &= arm.checkMotionDone();

				YARPTime::DelayInSeconds (0.1);
				timeout ++;
				if (timeout >= 50)
					finished = true;
			}

			if (m_delay[i] != 0)
				YARPTime::DelayInSeconds (double(m_delay[i])/1000.0);
		}
		else
		{
			MessageBox ("The controllers [arm and head] must be running before using this button", "Error!");
		}
	}

	MessageBox ("Sequence execution completed", "Information!");
}

void CSeqDlg::Body (void)
{
	/* */
	CTestControlDlg& p = *(CTestControlDlg *)GetParent();	
	int count = 0;

	while (!IsTerminated())
	{
		if (m_s[count] < 0 || m_s[count] >= N_POSTURES || count >= SEQUENCE_LEN)
			count = 0;

		if (p._headrunning && p._armrunning)
		{
			bool finished = false;
			head.setVelocities (p._headstorev[m_s[count]]);
			arm.setVelocities (p._armstorev[m_s[count]]);
			head.setPositions (p._headstore[m_s[count]]);
			arm.setPositions (p._armstore[m_s[count]]);

			YARPTime::DelayInSeconds (0.2);

			// wait.
			int timeout = 0;
			while (!finished)
			{
				finished = head.checkMotionDone();
				finished &= arm.checkMotionDone();

				YARPTime::DelayInSeconds (0.1);
				timeout ++;
				if (timeout >= 50)
					finished = true;
			}

			if (m_delay[count] != 0)
				YARPTime::DelayInSeconds (double(m_delay[count])/1000.0);
		}
	}
}

void CSeqDlg::OnButtonLoop() 
{
	/* diable all changes and start thread */
	HWND w;
	int i;
	// must be consecutive in resource.h
	for (i = 0; i < SEQUENCE_LEN; i++)
	{
		GetDlgItem(IDC_EDIT1+i, &w);
		::EnableWindow (w, FALSE);
		GetDlgItem(IDC_WAIT_1+i, &w);
		::EnableWindow (w, FALSE);
	}

	GetDlgItem(IDC_BUTTON_RUN, &w);
	::EnableWindow (w, FALSE);	

	GetDlgItem(IDC_BUTTON_LOOP, &w);
	::EnableWindow (w, FALSE);	

	UpdateData (TRUE);

	Begin ();
}

void CSeqDlg::OnButtonStopl() 
{
	End ();

	HWND w;
	int i;
	// must be consecutive in resource.h
	for (i = 0; i < SEQUENCE_LEN; i++)
	{
		GetDlgItem(IDC_EDIT1+i, &w);
		::EnableWindow (w, TRUE);
		GetDlgItem(IDC_WAIT_1+i, &w);
		::EnableWindow (w, TRUE);
	}

	GetDlgItem(IDC_BUTTON_RUN, &w);
	::EnableWindow (w, TRUE);

	GetDlgItem(IDC_BUTTON_LOOP, &w);
	::EnableWindow (w, TRUE);		
}
