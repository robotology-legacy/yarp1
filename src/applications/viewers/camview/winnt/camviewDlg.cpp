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
	/// 
	m_inport.Register (m_name);

	double start = -1;
	double cur = -1;
	int frame_no = 0;

	while (m_end_sema.PollingWait() != 1)
	{
		m_inport.Read();
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
		{
			if (m_img.GetWidth() != _logpolarParams::_stheta || m_img.GetHeight() != _logpolarParams::_srho - _logpolarParams::_sfovea)
			{
				/// falls back to cartesian mode.
				m_logp = false;
				continue;
			}

			if (m_remapped.GetWidth() != 256 || m_remapped.GetHeight() != 256)
			{
				m_remapped.Resize (256, 256, YARP_PIXEL_BGR);
			}

			if (m_flipped.GetWidth() != m_remapped.GetWidth() || m_flipped.GetHeight() != m_remapped.GetHeight())
			{
				m_flipped.Resize (m_remapped.GetWidth(), m_remapped.GetHeight(), m_remapped.GetID());
				///m_flipped.PeerCopy(m_img);
			}

			m_xxx.CastCopy (m_img);
			m_mapper.Logpolar2Cartesian (m_xxx, m_remapped);
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
	}
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
	: CDialog(CCamviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCamviewDlg)
	m_connection_name = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCamviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCamviewDlg)
	DDX_Control(pDX, IDOK, m_ctrl_quit);
	DDX_Control(pDX, IDC_NAME, m_ctrl_name);
	DDX_Text(pDX, IDC_NAME, m_connection_name);
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
	if (p->m_lp) m_receiver.AssumeLogpolar();

	UpdateData(FALSE);

	m_receiver.SetOwner (this);
	m_receiver.SetName ((LPCSTR)m_connection_name);
	
	m_receiver.Begin ();

	m_frozen = false;

	m_drawdib = DrawDibOpen (); 
	ASSERT (m_drawdib != NULL);

	m_receiver.SetPeriod (p->m_period);

	CRect wpos;
	GetWindowRect (&wpos);
	ClientToScreen (&wpos);

	if (p->m_x < 0) p->m_x = wpos.left;
	if (p->m_y < 0) p->m_y = wpos.top;

	MoveWindow (p->m_x, p->m_y, wpos.Width(), wpos.Height(), TRUE);

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
	}
	else
	{
		CPaintDC dc(this);

		unsigned char *dib = m_receiver.AcquireBuffer();

		CRect rect, rect2 = 0;
		GetClientRect (&rect);
		
		/// is this needed?
		dc.LPtoDP (&rect);

		if (m_ctrl_quit.m_hWnd != NULL)
		{
			m_ctrl_quit.GetWindowRect (&rect2);
		}

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

	m_receiver.End ();
	CDialog::OnClose();
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

	if (m_ctrl_name.m_hWnd != NULL && m_ctrl_quit.m_hWnd != NULL)
	{
		CRect rect, rect2;
		m_ctrl_name.GetWindowRect (&rect);
		m_ctrl_quit.GetWindowRect (&rect2);

		const int name_w = cx - 2*_BORDER - rect2.Width() - 2;

		m_ctrl_name.MoveWindow (_BORDER+1, cy-_BORDER-rect.Height()-1, name_w, rect.Height(), FALSE);
		m_ctrl_quit.MoveWindow (_BORDER+1 + name_w+1, cy-_BORDER-rect2.Height()-1, rect2.Width(), rect2.Height(), FALSE);

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
