// Simple ViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Simple Viewer.h"
#include "Simple ViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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
// CSimpleViewerDlg dialog

CSimpleViewerDlg::CSimpleViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpleViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimpleViewerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimpleViewerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSimpleViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CSimpleViewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleViewerDlg message handlers

BOOL CSimpleViewerDlg::OnInitDialog()
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
	m_bIsStarted = false;

	m_idParam = Set_Param( 1090,1090,
						   352, 288,
						   152, 252, 
							42,1090,
						   CUSTOM,
						   FITOUT);

//	Init_Destination();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSimpleViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSimpleViewerDlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSimpleViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSimpleViewerDlg::OnStart() 
{
	if(m_bIsStarted)
		return;
	else
	{
		Init_File();
		Init_Destination();
		m_bIsStarted = true;
		m_bStop = false;
//		AllocateMemory();
//		m_bAllocated = true;
		InvalidateRect(&m_rcRepaintArea);
		GetClientRect(&m_ClientRect);
		DWORD dwThread;

		m_hThread = CreateThread(NULL,
								 0,
								 VisualThread,
								 this,
								 0,
								 &dwThread);
	}
}

int CSimpleViewerDlg::Init_File()
{
	FILE * fin;
	char Path []  = "C:\\Temp\\From Talia\\Tables\\Giotto2.0\\";
	char File_Name [256];

	m_iPortAddress = Giotto_Init_Wrapper(m_idParam.Size_Theta,
								 m_idParam.Size_Rho,
								 "\\\\.\\LPTSIMPLE1",
								 0,
								 COLOR,
								 0x378,
								 &m_iCNumb);

	m_ucAcqImage = (unsigned char *)calloc(m_idParam.Size_LP, sizeof(unsigned char));
	m_ucDisplImage = (unsigned char *)calloc(m_idParam.Size_LP*3, sizeof(unsigned char));
	m_ucDisplImageRem = (unsigned char *)calloc(352*288*3, sizeof(unsigned char));
	m_iRemapLUT = (int *)malloc(352*288 * sizeof(int));

	sprintf(File_Name,"%s%s_%2.2f_%dx%d%s",Path,"RemapLUT",0.42,352,288,".gio");
	
	fin = fopen(File_Name,"rb");
	fread(m_iRemapLUT,sizeof(int),352*288,fin);
	fclose (fin);


	return m_iPortAddress;

}

int CSimpleViewerDlg::Init_Destination()
{
	m_biHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_biHeader.biCompression = BI_RGB;
	m_biHeader.biPlanes = 1;
	m_biHeader.biBitCount = 24;
	 

	m_biHeader.biWidth = m_idParam.Size_Theta;
	m_biHeader.biHeight = m_idParam.Size_Rho;
	m_biHeader.biSizeImage = 3 * m_idParam.Size_LP;


	m_biHeader.biClrImportant = 0;
	m_biHeader.biClrUsed = 0;
	m_biHeader.biXPelsPerMeter = 0;
	m_biHeader.biYPelsPerMeter = 0;

	m_biHeaderRem.biSize = sizeof(BITMAPINFOHEADER);
	m_biHeaderRem.biCompression = BI_RGB;
	m_biHeaderRem.biPlanes = 1;
	m_biHeaderRem.biBitCount = 24;
	 

	m_biHeaderRem.biWidth = 352;
	m_biHeaderRem.biHeight = 288;
	m_biHeaderRem.biSizeImage = 3 * 352 * 288;


	m_biHeaderRem.biClrImportant = 0;
	m_biHeaderRem.biClrUsed = 0;
	m_biHeaderRem.biXPelsPerMeter = 0;
	m_biHeaderRem.biYPelsPerMeter = 0;

	m_hDrawDib = DrawDibOpen();
	m_hDrawDibRem = DrawDibOpen();

	DrawDibBegin(m_hDrawDib,
				 m_hMyDC,
				 -1, 
				 -1,
				 &m_biHeader,
				 m_biHeader.biWidth,
				 -m_biHeader.biHeight,
				 0);

	DrawDibBegin(m_hDrawDibRem,
				 m_hMyDC,
				 -1, 
				 -1,
				 &m_biHeaderRem,
				 m_biHeaderRem.biWidth,
				 -m_biHeaderRem.biHeight,
				 0);

	return 0;
}

DWORD WINAPI VisualThread(LPVOID param)
{
	CSimpleViewerDlg* pMainWnd = (CSimpleViewerDlg*) param;

	pMainWnd->Init_Destination();
	pMainWnd->EndedThread = false;
	while(pMainWnd->EndedThread == false)
	{
/*		if (pMainWnd->Calib)
		{
			pMainWnd->SetCal();
			pMainWnd->Calib = FALSE;
		}
*/
		pMainWnd->Acquire_Frame();

		pMainWnd->Send_Frame	();

		if (pMainWnd->m_bStop)
		{
			pMainWnd->Close_Acquisition();
			pMainWnd->Close_Display();
//			pMainWnd->Free_Memory();
			pMainWnd->EndedThread = true;
			ExitThread(1);
		}

		Sleep(0);
	}
	return 0;
}

int CSimpleViewerDlg::Acquire_Frame()
{
	Acquire_From_Camera();
	return 0;
}

void CSimpleViewerDlg::Send_Frame()
{
		m_hMyDC = ::GetDC(m_hWnd);
		
		DrawDibDraw (m_hDrawDib,
					 m_hMyDC,
					 m_ClientRect.left+80,
					 m_ClientRect.top+30,
					 -1,
					 -1,
					 &m_biHeader,
					 m_ucDisplImage,
					 0,
					 0,
					 m_biHeader.biWidth,
					 -m_biHeader.biHeight,
					 DDF_SAME_DRAW);

		DrawDibDraw (m_hDrawDibRem,
					 m_hMyDC,
					 //m_idParam.Displacement_X,
					 //m_idParam.Displacement_Y,
					 m_ClientRect.left+30,
					 m_ClientRect.top+200,
					 -1,
					 -1,
					 &m_biHeaderRem,
//					 m_idParam.Image_to_Display,
					 m_ucDisplImageRem,
					 0,
					 0,
					 m_biHeaderRem.biWidth,
					 -m_biHeaderRem.biHeight,
					 DDF_SAME_DRAW);
		
		::ReleaseDC(m_hWnd,m_hMyDC);
		
		Sleep(1);

}

void CSimpleViewerDlg::Close_Acquisition()
{

}

void CSimpleViewerDlg::Close_Display()
{
	DrawDibEnd(m_hDrawDib);
	DrawDibClose (m_hDrawDib);
}

void CSimpleViewerDlg::Acquire_From_Camera()
{
	int bytes;
	int i,j;
	struct timeval elap_time;

	bytes = Giotto_Acq_Wrapper(m_ucAcqImage,
					   &elap_time,
					   &i,
					   &m_iCNumb,
					   m_iPortAddress);

	for (i=0; i<m_idParam.Size_LP; i++)
	{
		m_ucDisplImage[3*i] = m_ucAcqImage[i];
		m_ucDisplImage[3*i+1] = m_ucAcqImage[i];
		m_ucDisplImage[3*i+2] = m_ucAcqImage[i];
	}

	Remap(m_ucDisplImageRem,m_ucDisplImage,&m_idParam,m_iRemapLUT);

//	for (j=0; j<m_idParam.Size_Rho; j++)
//		for (i=0; i<m_idParam.Size_Theta; i++)
//			memcpy(&m_ucDisplImage[3*(j*m_idParam.Size_Theta+i)] = buffer; 

}

//DEL void CSimpleViewerDlg::Set_Parameters(Image_Data *param)
//DEL {
//DEL 	param->Size_Fovea = 42;
//DEL 	param->Size_Rho   = 152;
//DEL 	param->Size_Theta = 252;
//DEL 
//DEL 	param->Size_LP = param->Size_Rho * param->Size_Theta;
//DEL 
//DEL 	param->Size_X_Remap = 352;
//DEL 	param->Size_Y_Remap = 288;
//DEL 	param->Size_Img_Remap = param->Size_X_Remap * param->Size_Y_Remap;
//DEL }


