// camviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "camview.h"
#include "camviewDlg.h"
#include "newrateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int _AVE = 20;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

void CRecv::Body (void)
{
	using namespace _logpolarParams;

	/// 
	m_inport.Register (m_name);

	double start = -1;
	double cur = -1;
	int frame_no = 0;

	while (!IsTerminated())
	{
		if (m_inport.Read())
		{
			m_img.Refer (m_inport.Content());

			/// cartesian.
			if (!m_logp)
			{
				if (m_flipped.GetWidth() != m_img.GetWidth() || m_flipped.GetHeight() != m_img.GetHeight())
				{
					m_flipped.Resize (m_img.GetWidth(), m_img.GetHeight(), m_img.GetID());
				}

				YARPSimpleOperation::Flip (m_img, m_flipped);

				m_mutex.Wait();
				if (m_flipped.GetWidth() != m_x || m_flipped.GetHeight() != m_y)
				{
					m_converter.Resize (m_flipped);
					m_x = m_flipped.GetWidth ();
					m_y = m_flipped.GetHeight ();
				}

				if (!m_frozen)
				{
					/// prepare the DIB to display.
					m_converter.ConvertToDIB (m_flipped);
				}
			}
			/// or logpolar.
			else
			if (m_logp && !m_fov)
			{
				if (m_img.GetWidth() != _stheta || m_img.GetHeight() != _srho)
				{
					/// falls back to cartesian mode.
					m_logp = false;
					continue;
				}

				if (m_remapped.GetWidth() != 256 || m_remapped.GetHeight() != 256)
				{
					m_remapped.Resize (256, 256, YARP_PIXEL_BGR);
				}

				if (m_colored.GetWidth() != _stheta || m_colored.GetHeight() != _srho)
				{
					m_colored.Resize (_stheta, _srho);
				}

				if (m_flipped.GetWidth() != m_remapped.GetWidth() || m_flipped.GetHeight() != m_remapped.GetHeight())
				{
					m_flipped.Resize (m_remapped.GetWidth(), m_remapped.GetHeight(), m_remapped.GetID());
				}

				m_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)m_img, m_colored);
				m_mapper.Logpolar2Cartesian (m_colored, m_remapped);
				YARPSimpleOperation::Flip (m_remapped, m_flipped);

				m_mutex.Wait();
				if (m_flipped.GetWidth() != m_x || m_flipped.GetHeight() != m_y)
				{
					m_converter.Resize (m_flipped);
					m_x = m_flipped.GetWidth ();
					m_y = m_flipped.GetHeight ();
				}

				if (!m_frozen)
				{
					/// prepare the DIB to display.
					m_converter.ConvertToDIB (m_flipped);
				}
			}
			else	/// logpolar fovea.
			if (m_logp && m_fov)
			{
				if (m_img.GetWidth() != _stheta || m_img.GetHeight() != _srho)
				{
					/// falls back to cartesian mode.
					m_logp = false;
					continue;
				}

				if (m_remapped.GetWidth() != 128 || m_remapped.GetHeight() != 128)
				{
					m_remapped.Resize (128, 128, YARP_PIXEL_BGR);
				}

				if (m_colored.GetWidth() != _stheta || m_colored.GetHeight() != _srho)
				{
					m_colored.Resize (_stheta, _srho);
				}

				if (m_flipped.GetWidth() != m_remapped.GetWidth() || m_flipped.GetHeight() != m_remapped.GetHeight())
				{
					m_flipped.Resize (m_remapped.GetWidth(), m_remapped.GetHeight(), m_remapped.GetID());
				}

				m_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)m_img, m_colored);
				m_mapper.Logpolar2CartesianFovea (m_colored, m_remapped);
				YARPSimpleOperation::Flip (m_remapped, m_flipped);

				m_mutex.Wait();
				if (m_flipped.GetWidth() != m_x || m_flipped.GetHeight() != m_y)
				{
					m_converter.Resize (m_flipped);
					m_x = m_flipped.GetWidth ();
					m_y = m_flipped.GetHeight ();
				}

				if (!m_frozen)
				{
					/// prepare the DIB to display.
					m_converter.ConvertToDIB (m_flipped);
				}
			}
			frame_no ++;
			if (start == -1)
			{
				start = cur = YARPTime::GetTimeAsSeconds();
			}

			if ((frame_no % _AVE) == 0)
			{
				cur = YARPTime::GetTimeAsSeconds();
				m_est_interval = (cur - start) / _AVE;
				start = cur;
			}

			/// copy it somewhere and fire a WM_PAINT event.
			m_owner->InvalidateRect (m_rect, FALSE);

			m_mutex.Post();

			if (m_period != 0)
			{
				/// approximate, don't want to do anything more complicate than this.
				YARPTime::DelayInSeconds ((m_period - 2)* 1e-3);
			}

		}	/// if Read() == true
		else
		{
			/// ask for thread termination.
			AskForEnd();
		}
	}	/// while (!IsTerminated())
}

void CRecv::SaveCurrentFrame (const char *filename)
{
	m_mutex.Wait ();
	m_converter.SaveDIB (filename);
	m_mutex.Post ();
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCamviewDlg dialog

CCamviewDlg::CCamviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCamviewDlg::IDD, pParent),
	m_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
{
	//{{AFX_DATA_INIT(CCamviewDlg)
	m_connection_name = _T("");
	m_image_x = 0;
	m_image_y = 0;
	m_output_connection = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCamviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCamviewDlg)
	//DDX_Control(pDX, IDC_NAME, m_ctrl_name);
	DDX_Text(pDX, IDC_NAME, m_connection_name);
	DDX_Text(pDX, IDC_X, m_image_x);
	DDX_Text(pDX, IDC_Y, m_image_y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCamviewDlg, CDialog)
	//{{AFX_MSG_MAP(CCamviewDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnQuit)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_WM_SIZE()
	ON_COMMAND(ID_IMAGE_FREEZE, OnImageFreeze)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_FREEZE, OnUpdateImageFreeze)
	ON_COMMAND(ID_IMAGE_NEWRATE, OnImageNewrate)
	ON_COMMAND(ID_FILE_SAVEIMAGE, OnFileSaveimage)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEIMAGE, OnUpdateFileSaveimage)
	ON_COMMAND(ID_IMAGE_SHOWINTERVAL, OnImageShowinterval)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCamviewDlg message handlers

BOOL CCamviewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CCamviewApp *p = ((CCamviewApp *)AfxGetApp());
	m_connection_name = p->m_portname;
	m_output_connection = p->m_out_portname;
	if (p->m_lp && m_receiver.TablesOk()) m_receiver.AssumeLogpolar();
	if (p->m_fov) m_receiver.AssumeDisplayFovea();

	UpdateData(FALSE);

	m_receiver.SetOwner (this);
	m_receiver.SetName ((LPCSTR)m_connection_name);
	
	m_receiver.Begin ();

	m_outPort.Register(m_output_connection);

	m_frozen = false;

	m_drawdib = DrawDibOpen (); 
	ASSERT (m_drawdib != NULL);

	m_receiver.SetPeriod (p->m_period);

	CRect wpos;
	GetWindowRect (&wpos);
	ClientToScreen (&wpos);

	if (p->m_x < 0) p->m_x = wpos.left;
	if (p->m_y < 0) p->m_y = wpos.top;
	if (p->m_width < 0) p->m_width = wpos.Width();
	if (p->m_height < 0) p->m_height = wpos.Height();

	MoveWindow (p->m_x, p->m_y, p->m_width, p->m_height, TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCamviewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

///
///
///
void CopyToScreen(HDRAWDIB hDD, HDC hDC, unsigned char *img, int destX, int destY, double zoomX, double zoomY)
{
	ACE_ASSERT (img != NULL);				

	BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER *)img;
	
	int X = dibhdr->biWidth; 
	int Y = dibhdr->biHeight;
	int sizeX = int(X * zoomX);
	int sizeY = int(Y * zoomY);
	
	/// LATER: it should be a bit more generic.
	if (dibhdr->biBitCount == 8)
	{
		DrawDibDraw(
		  hDD, hDC,
		  destX, destY,
		  sizeX, sizeY,
		  (BITMAPINFOHEADER *)img,
		  img + sizeof(RGBQUAD) * 256 + sizeof(BITMAPINFOHEADER),
		  0, 0,                 
		  X, Y,
		  0);
	}
	else
	{
		DrawDibDraw(
		  hDD, hDC,
		  destX, destY,
		  sizeX, sizeY,
		  (BITMAPINFOHEADER *)img,
		  img + sizeof(BITMAPINFOHEADER),
		  0, 0,                 
		  X, Y,
		  0);
	}
}

#define _BORDER 10


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCamviewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		m_zx = 1;
		m_zy = 1;
		m_x = x;
		m_y = x;
		m_fx = 0;
		m_fy = 0;
	}
	else
	{
		CPaintDC dc(this);

		unsigned char *dib = m_receiver.AcquireBuffer();

		CRect rect, rect2 = 0;
		GetClientRect (&rect);
		
		/// is this needed?
		dc.LPtoDP (&rect);

		GetDlgItem(IDC_NAME)->GetWindowRect(&rect2);
		/*
		if (m_connection_name.m_hWnd != NULL)
		{
			m_connection_name.GetWindowRect (&rect2);
		}*/
		
		double zx = double(rect.Width() - 2 * _BORDER) / double(m_receiver.GetWidth());
		double zy = double(rect.Height() - 2 * _BORDER - rect2.Height() - 2) / double(m_receiver.GetHeight());

		if (zx > zy)
		{
			zx = zy;
		}

		int fx = int (m_receiver.GetWidth() * zx + .5);
		int fy = int (m_receiver.GetHeight() * zx +.5);

		int x = ((rect.Width() - 2 * _BORDER) - fx) / 2 + _BORDER;
		int y = ((rect.Height() - 2 * _BORDER - rect2.Height() - 2) - fy) / 2 + _BORDER;

		if (dib != NULL)
			CopyToScreen(m_drawdib, dc.GetSafeHdc(), dib, x, y, zx, zx);
		else
		{
			CGdiObject * old = dc.SelectStockObject (GRAY_BRUSH);
			dc.Rectangle (x, y, x+fx, y+fy);
			dc.SelectObject (old);
		}
		
		m_receiver.ReleaseBuffer();

		m_receiver.SetPaintRectangle (x, y, x+fx, y+fy);

		m_x = x;
		m_y = y;
		m_zx = zx;
		m_zy = zx;	// should be zy but we use zx
		m_fx = fx;
		m_fy = fy;

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCamviewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCamviewDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if (!bSysMenu)
	{
		ASSERT(pPopupMenu != NULL);
		// check the enabled state of various menu items
		CCmdUI state;
		state.m_pMenu = pPopupMenu;
		ASSERT(state.m_pOther == NULL);
		state.m_nIndexMax = pPopupMenu->GetMenuItemCount();

		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
		{
			state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
			if (state.m_nID == 0)
				continue; // menu separator or invalid cmd - ignore it
			ASSERT(state.m_pOther == NULL);
			ASSERT(state.m_pMenu != NULL);
			if (state.m_nID == (UINT)-1)
			{
				// possibly a popup menu, route to first item of that popup
				state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
				if (state.m_pSubMenu == NULL || 
					(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
					state.m_nID == (UINT)-1)
				{
					continue; // first item of popup can't be routed to
				}
				state.DoUpdate(this, FALSE);  // popups are never auto disabled
			}
			else
			{
				// normal menu item
				// Auto enable/disable if command is _not_ a system command
				state.m_pSubMenu = NULL;
				state.DoUpdate(this, state.m_nID < 0xF000);
			}
		}
	}	
}

void CCamviewDlg::OnClose() 
{
	DrawDibClose (m_drawdib);

	m_receiver.AskForEnd ();
	m_receiver.TryClosePort ();
	m_receiver.Join ();

	m_outPort.Unregister ();

	CDialog::OnClose ();
}

void CCamviewDlg::OnQuit() 
{
	PostMessage (WM_CLOSE);
}

void CCamviewDlg::OnFileExit() 
{
	OnQuit ();
}

void CCamviewDlg::OnHelpAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CCamviewDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (GetDlgItem(IDC_NAME)!= NULL)
	{
		CRect rect, rect2, rect3;
		GetDlgItem(IDC_NAME)->GetWindowRect (&rect);
		GetDlgItem(IDC_X)->GetWindowRect (&rect2);
		GetDlgItem(IDC_Y)->GetWindowRect (&rect3);

		const int name_w = cx - 2*_BORDER - rect2.Width() - rect3.Width() - 4;

		GetDlgItem(IDC_NAME)->MoveWindow (_BORDER+1, cy-_BORDER-rect.Height()-1, name_w, rect.Height(), FALSE);
		GetDlgItem(IDC_X)->MoveWindow (_BORDER+1 + name_w+1, cy-_BORDER-rect2.Height()-1, rect2.Width(), rect2.Height(), FALSE);
		GetDlgItem(IDC_Y)->MoveWindow (_BORDER+1 + name_w+1 + rect2.Height() + 8, cy-_BORDER-rect3.Height()-1, rect3.Width(), rect3.Height(), FALSE);

		InvalidateRect (NULL, TRUE);
	}
}

void CCamviewDlg::OnImageFreeze() 
{
	m_frozen = !m_frozen;	
	m_receiver.Freeze (m_frozen);
}

void CCamviewDlg::OnUpdateImageFreeze(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_frozen);	
}

void CCamviewDlg::OnImageNewrate() 
{
	CNewRateDlg dlgRate;

	dlgRate.m_period = m_receiver.GetPeriod ();

	int nResponse = dlgRate.DoModal();

	if (nResponse == IDOK)
	{
		m_receiver.SetPeriod (dlgRate.m_period);
	}
}

void CCamviewDlg::OnFileSaveimage() 
{
	OnImageFreeze ();

	CFileDialog fileDlg(FALSE,		/// save as dialog
						"bmp",		/// default ext
						NULL,		/// default filename
						OFN_OVERWRITEPROMPT, /// flags
						"Windows bitmap files (*.bmp)|*.bmp||", 
						this);

	int nResponse = fileDlg.DoModal();
	if (nResponse == IDOK)
	{
		m_receiver.SaveCurrentFrame (fileDlg.m_ofn.lpstrFile);
	}

	OnImageFreeze ();
}

void CCamviewDlg::OnUpdateFileSaveimage(CCmdUI* pCmdUI) 
{
	unsigned char *dib = m_receiver.AcquireBuffer();
	m_receiver.ReleaseBuffer();
	pCmdUI->Enable (dib != NULL);
}

void CCamviewDlg::OnImageShowinterval() 
{
	CString string;
	string.Format ("Estimated interval during last %d cycles: %.3f\0", _AVE, m_receiver.GetEstimatedInterval());
	MessageBox (string);
}

void CCamviewDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_image_x = (point.x - m_x)/m_zx;
	m_image_y = (point.y - m_y)/m_zy;

	if (m_image_x < 0) m_image_x = 0;
	if (m_image_x > m_receiver.GetWidth()) m_image_x = m_receiver.GetWidth();
	
	if (m_image_y < 0) m_image_y = 0;
	if (m_image_y > m_receiver.GetHeight()) m_image_y = m_receiver.GetHeight();

	m_outPort.Content()[0] = m_image_x;
	m_outPort.Content()[1] = m_image_y;
	m_outPort.Write();

	UpdateData(FALSE);
	CDialog::OnLButtonDblClk(nFlags, point);
}
