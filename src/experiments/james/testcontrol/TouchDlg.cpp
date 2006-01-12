// TouchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testcontrol.h"
#include "TouchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern YARPEsdDaqDeviceDriver touch;

/////////////////////////////////////////////////////////////////////////////
// CTouchDlg dialog


CTouchDlg::CTouchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTouchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTouchDlg)
	//}}AFX_DATA_INIT
	ACE_OS::memset (m_c, 0, sizeof(short) * MAX_CHANNELS);
	m_mask = 0;
	m_count = 0;
}


void CTouchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTouchDlg)
	//}}AFX_DATA_MAP

	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		DDX_Control(pDX, IDC_EDIT_C1+i, m_edt[i]);
		DDX_Text(pDX, IDC_EDIT_C1+i, m_c[i]);
	}
}


BEGIN_MESSAGE_MAP(CTouchDlg, CDialog)
	//{{AFX_MSG_MAP(CTouchDlg)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTouchDlg message handlers

void CTouchDlg::OnButtonHide() 
{
	ShowWindow (SW_HIDE);	
}

void CTouchDlg::EnableInterface()
{
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		m_edt[i].EnableWindow(TRUE);
	}
}

void CTouchDlg::DisableInterface()
{
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		m_edt[i].EnableWindow(FALSE);
	}
}

void CTouchDlg::SetMask (int newmask)
{
	m_mask = newmask;
	m_count = 0;
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		if (newmask & 0x1)
			m_count++;
		newmask >>= 1;
	}
}

// MUST be called when the device driver is properly initialized.
void CTouchDlg::UpdateInterface()
{
	int ret = YARP_OK;

	ret = touch.IOCtl (CMDAIReadScan, (void *)m_c);
	if (ret != YARP_OK)
	{
		// silently ignores the issue.
		ACE_OS::memset (m_c, 0, sizeof(short) * MAX_CHANNELS);
	}
	else
	{
		int val = m_mask;
		int i, j;
		for (i = 0, j = 0; i < MAX_CHANNELS; i++)
		{
			if (val & 0x1)
			{
				m_c[MAX_CHANNELS-1-i] = m_c[m_count-1-j];
				j++;
			}
			else
			{
				m_c[MAX_CHANNELS-1-i] = 0;
			}

			val >>= 1;
		}

		UpdateData (FALSE);
	}
}