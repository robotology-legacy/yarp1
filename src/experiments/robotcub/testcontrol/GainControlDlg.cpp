// GainControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testcontrol.h"
#include "GainControlDlg.h"
#include "TestControlDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern YARPHead head;
extern YARPArm arm;


/////////////////////////////////////////////////////////////////////////////
// CGainControlDlg dialog


CGainControlDlg::CGainControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGainControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGainControlDlg)
	m_dgain = 0.0;
	m_igain = 0.0;
	m_ilimit = 0.0;
	m_offset = 0.0;
	m_pgain = 0.0;
	m_shift = 0.0;
	m_tlimit = 0.0;
	m_max = 0;
	m_min = 0;
	//}}AFX_DATA_INIT

	//m_parent = (CTestControlDlg *)pParent;
}


void CGainControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGainControlDlg)
	DDX_Control(pDX, IDC_BUTTON_WRITEFLASH, m_writeflash_bt);
	DDX_Control(pDX, IDC_BUTTON_READFLASH, m_readflash_bt);
	DDX_Control(pDX, IDC_COMBO_BUS, m_bus_combo);
	DDX_Control(pDX, IDC_EDIT_TLIMIT, m_tlimit_edt);
	DDX_Control(pDX, IDC_EDIT_SHIFT, m_shift_edt);
	DDX_Control(pDX, IDC_EDIT_PGAIN, m_pgain_edt);
	DDX_Control(pDX, IDC_EDIT_OFFSET, m_offset_edt);
	DDX_Control(pDX, IDC_EDIT_MIN, m_min_edt);
	DDX_Control(pDX, IDC_EDIT_MAX, m_max_edt);
	DDX_Control(pDX, IDC_EDIT_ILIMIT, m_ilimit_edt);
	DDX_Control(pDX, IDC_EDIT_IGAIN, m_igain_edt);
	DDX_Control(pDX, IDC_EDIT_DGAIN, m_dgain_edt);
	DDX_Control(pDX, IDC_COMBO_AXIS, m_axis_combo);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_update_bt);
	DDX_Control(pDX, IDC_BUTTON_SETMINMAX, m_setminmax_bt);
	DDX_Text(pDX, IDC_EDIT_DGAIN, m_dgain);
	DDV_MinMaxDouble(pDX, m_dgain, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_IGAIN, m_igain);
	DDV_MinMaxDouble(pDX, m_igain, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_ILIMIT, m_ilimit);
	DDV_MinMaxDouble(pDX, m_ilimit, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_OFFSET, m_offset);
	DDV_MinMaxDouble(pDX, m_offset, -32768., 32767.);
	DDX_Text(pDX, IDC_EDIT_PGAIN, m_pgain);
	DDV_MinMaxDouble(pDX, m_pgain, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_SHIFT, m_shift);
	DDV_MinMaxDouble(pDX, m_shift, 0., 16.);
	DDX_Text(pDX, IDC_EDIT_TLIMIT, m_tlimit);
	DDV_MinMaxDouble(pDX, m_tlimit, 0., 32767.);
	DDX_Text(pDX, IDC_EDIT_MAX, m_max);
	DDX_Text(pDX, IDC_EDIT_MIN, m_min);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGainControlDlg, CDialog)
	//{{AFX_MSG_MAP(CGainControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_SETMINMAX, OnButtonSetminmax)
	ON_CBN_SELENDOK(IDC_COMBO_AXIS, OnSelendokComboAxis)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELENDOK(IDC_COMBO_BUS, OnSelendokComboBus)
	ON_BN_CLICKED(IDC_BUTTON_READFLASH, OnButtonReadflash)
	ON_BN_CLICKED(IDC_BUTTON_WRITEFLASH, OnButtonWriteflash)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGainControlDlg message handlers

void CGainControlDlg::OnButtonHide() 
{
	ShowWindow (SW_HIDE);	
}

void CGainControlDlg::EnableInterface()
{
	m_tlimit_edt.EnableWindow();
	m_shift_edt.EnableWindow();
	m_pgain_edt.EnableWindow();
	m_offset_edt.EnableWindow();
	m_min_edt.EnableWindow();
	m_max_edt.EnableWindow();
	m_ilimit_edt.EnableWindow();
	m_igain_edt.EnableWindow();
	m_dgain_edt.EnableWindow();
	m_axis_combo.EnableWindow();
	m_bus_combo.EnableWindow();
	m_update_bt.EnableWindow();
	m_setminmax_bt.EnableWindow();
	m_readflash_bt.EnableWindow();
	m_writeflash_bt.EnableWindow();
}

void CGainControlDlg::DisableInterface()
{
	m_tlimit_edt.EnableWindow(FALSE);
	m_shift_edt.EnableWindow(FALSE);
	m_pgain_edt.EnableWindow(FALSE);
	m_offset_edt.EnableWindow(FALSE);
	m_min_edt.EnableWindow(FALSE);
	m_max_edt.EnableWindow(FALSE);
	m_ilimit_edt.EnableWindow(FALSE);
	m_igain_edt.EnableWindow(FALSE);
	m_dgain_edt.EnableWindow(FALSE);
	m_axis_combo.EnableWindow(FALSE);
	m_bus_combo.EnableWindow(FALSE);
	m_update_bt.EnableWindow(FALSE);
	m_setminmax_bt.EnableWindow(FALSE);
	m_readflash_bt.EnableWindow(FALSE);
	m_writeflash_bt.EnableWindow(FALSE);
}

void CGainControlDlg::OnButtonUpdate() 
{
	UpdateData(TRUE);

	const int index = m_axis_combo.GetCurSel();
	const int bus = m_bus_combo.GetCurSel();

	if (index != CB_ERR && bus != CB_ERR)
	{
		LowLevelPID pid;
		pid.KP = m_pgain;
		pid.KD = m_dgain;
		pid.SHIFT = m_shift;
		pid.OFFSET = m_offset;
		pid.I_LIMIT = m_ilimit;
		pid.KI = m_igain;
		pid.T_LIMIT = m_tlimit;

		switch (bus)
		{
		case 0:
			head.setPID(index, pid);
			head.setSoftwareLimits (index, m_min, m_max);
			break;

		case 1:
			break;
		}
	}	
}

void CGainControlDlg::OnButtonSetminmax() 
{
	UpdateData (TRUE);

	const int index = m_axis_combo.GetCurSel();
	const int bus = m_bus_combo.GetCurSel();

	if (index >= MAX_HEAD_JNTS || 
		index == CB_ERR || 
		bus < 0 || 
		bus > 1 || 
		bus == CB_ERR)
	{
		return;
	}
	
	switch (bus)
	{
	case 0:	
		head.setSoftwareLimits (index, m_min, m_max);
		break;

	case 1:
		break;
	}
}

void CGainControlDlg::UpdateAxisParams (int bus, int axis)
{
	LowLevelPID pid;
	m_min = m_max = -1;
	double tmin = -1, tmax = -1;

	switch (bus)
	{
	case 0:
		{
			// head.
			memset (&pid, 0, sizeof(LowLevelPID));
			head.getPID (axis, pid);
			head.getSoftwareLimits (axis, tmin, tmax);
			if (tmin > 0)
				m_min = int(tmin + .5);
			else
				m_min = int(tmin - .5);
			if (tmax > 0) 
				m_max = int(tmax + .5);
			else
				m_max = int(tmax - .5);
		}
		break;
	case 1:
		{
			// arm and hand.
			memset (&pid, 0, sizeof(LowLevelPID));
		}
		break;

	default:
		MessageBox ("I bet there's an issue with the bus selection", "Error!");
		break;
	}

	m_pgain = pid.KP;
	m_dgain = pid.KD;
	m_shift = pid.SHIFT;
	m_offset = pid.OFFSET;
	m_ilimit = pid.I_LIMIT;
	m_igain = pid.KI;
	m_tlimit = pid.T_LIMIT;

	UpdateData(FALSE);
}

void CGainControlDlg::OnSelendokComboAxis() 
{
	const int index = m_axis_combo.GetCurSel();
	const int bus = m_bus_combo.GetCurSel();

	if ((bus == 0 && index >= MAX_HEAD_JNTS) || 
		(bus == 1 && index >= MAX_ARM_JNTS) ||
		index == CB_ERR || 
		bus < 0 
		|| bus > 1 
		|| bus == CB_ERR)
	{
		m_axis_combo.SetCurSel(-1);
		m_bus_combo.SetCurSel(0);
	}
	else
	{
		UpdateAxisParams (bus, index);
		m_axis_combo.SetCurSel(index);
		m_bus_combo.SetCurSel(bus);
	}
}

void CGainControlDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if (bShow)
	{
		m_bus_combo.SetCurSel(0);
	}
}

void CGainControlDlg::OnSelendokComboBus() 
{
	const int bus = m_bus_combo.GetCurSel();
	if (bus == CB_ERR || bus < 0 || bus > 1)
	{
		m_axis_combo.SetCurSel(-1);
		m_bus_combo.SetCurSel(-1);
	}
	else
	{
		m_axis_combo.SetCurSel(-1);
		m_bus_combo.SetCurSel(bus);
	}
}

void CGainControlDlg::OnButtonReadflash() 
{
	const int axis = m_axis_combo.GetCurSel();
	const int bus = m_bus_combo.GetCurSel();

	if (axis == CB_ERR)
		return;

	switch (bus)
	{
	case 0:
		head.loadFromBootMemory(axis);
		break;

	case 1:
		break;
	}
}

void CGainControlDlg::OnButtonWriteflash() 
{
	const int axis = m_axis_combo.GetCurSel();
	const int bus = m_bus_combo.GetCurSel();

	if (axis == CB_ERR)
		return;

	switch (bus)
	{
	case 0:
		head.saveToBootMemory(axis);
		break;

	case 1:
		break;
	}
}
