// camviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "camview.h"
#include "camviewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

void CRecv::Body (void)
{
	/// 
	m_inport.Register (m_name);

	while (1)
	{
		m_inport.Read();
		m_img.Refer (m_inport.Content());

		m_mutex.Wait();
		if (m_img.GetWidth() != m_x || m_img.GetHeight() != m_y)
		{
			m_converter.Resize (m_img);
		}

		/// prepare the DIB to display.
		m_converter.ConvertToDIB (m_img);
		m_mutex.Post();

		/// copy it somewhere and fire a WM_PAINT event.
		m_owner->InvalidateRect (NULL, FALSE);
	}
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
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCamviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCamviewDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
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
	m_receiver.SetOwner (this);
	m_receiver.SetName ((LPCSTR)(((CCamviewApp *)AfxGetApp())->m_portname));
	
	m_receiver.Begin ();

	/// required to set the stretch type.
	CPaintDC dc(this);
	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);  

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
void CopyToScreen(HDC hDC, unsigned char *img, int destX, int destY, double zoomX, double zoomY)
{
	ACE_ASSERT (img != NULL);				

	BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER *)img;
	
	int X = dibhdr->biWidth; 
	int Y = dibhdr->biHeight;
	int sizeX = int(X * zoomX);
	int sizeY = int(Y * zoomY);
	
	/// LATER: this is ok only for color images.
	StretchDIBits (
		hDC,
		destX, destY,
		sizeX, sizeY,
		0,0,
		X, Y,
		img + sizeof(BITMAPINFOHEADER),
		(BITMAPINFO *)img,
		DIB_RGB_COLORS,
		SRCCOPY);
}


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
		CopyToScreen(dc.GetSafeHdc(), dib, 0, 0, 1.0, 1.0);
		m_receiver.ReleaseBuffer();

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
	m_receiver.End ();
	CDialog::OnClose();
}

void CCamviewDlg::OnQuit() 
{
	PostMessage (WM_CLOSE);
}
