// cameraTuneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cameraTune.h"
#include "cameraTuneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PI 3.1415926535
#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )

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
// CCameraTuneDlg dialog

CCameraTuneDlg::CCameraTuneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraTuneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCameraTuneDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCameraTuneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCameraTuneDlg)
	DDX_Control(pDX, IDC_START_CAMERA, m_StartButton);
	DDX_Control(pDX, IDC_OPTIONS, m_OptionButton);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCameraTuneDlg, CDialog)
	//{{AFX_MSG_MAP(CCameraTuneDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SHOW_CAMERA, OnShowCamera)
	ON_BN_CLICKED(IDC_SHOW_EDGES, OnShowEdges)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_START_CAMERA, OnStartCamera)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CALIBRATE, OnCalibrate)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraTuneDlg message handlers

BOOL CCameraTuneDlg::OnInitDialog()
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
	initMembers();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCameraTuneDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCameraTuneDlg::OnPaint() 
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
HCURSOR CCameraTuneDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCameraTuneDlg::initMembers()
{
	CameraDialog.Create(CLiveCameraDlg::IDD, this);
	EdgesDialog.Create(CLiveEdgesDlg::IDD, this);

	bLiveCamera = false;
	bLiveEdges = false;
	setupOptions();
	EdgesDialog.flt.resize(options.sizeX, options.sizeY);
	EdgesDialog.edgeImg.Resize(options.sizeX, options.sizeY);
	hough_flt.resize(options.thetaSize, options.rhoSize);
	img.Resize (options.sizeX, options.sizeY);
	img_mono.Resize (options.sizeX, options.sizeY);
	m_timerID = NULL;
	m_ProgBar.SetRange(0, options.nImages);
	GetDlgItem(IDC_PROG_DESC)->SetWindowText("");
	GetDlgItem(IDC_CALIBRATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOW_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOW_EDGES)->EnableWindow(FALSE);
	img_buffer = new YARPImageOf<YarpPixelBGR> [options.nImages];
	for (int i=0; i< options.nImages; i++)
		img_buffer[i].Resize (options.sizeX, options.sizeY);
	
}

void CCameraTuneDlg::OnShowCamera() 
{
	if (bLiveCamera)
	{
		CameraDialog.ShowWindow(SW_HIDE);
		bLiveCamera = false;
	}
	else
	{
		
		if (m_timerID == NULL)
		{
			m_timerID = SetTimer(321, options.refresh, NULL);
			_ASSERT (m_timerID != 0);
		}
		
		CameraDialog.ShowWindow(SW_RESTORE);
		bLiveCamera = true;
	}
	
	if (!bLiveCamera && !bLiveEdges && (m_timerID != NULL) ) 
	{
		KillTimer (m_timerID);
		m_timerID = NULL;
	}	
	
}

void CCameraTuneDlg::OnShowEdges() 
{
	if (bLiveEdges)
	{
		EdgesDialog.ShowWindow(SW_HIDE);
		bLiveEdges = false;
	}
	else
	{
		
		if (m_timerID == NULL)
		{
			m_timerID = SetTimer(321, options.refresh, NULL);
			_ASSERT (m_timerID != 0);
		}
		EdgesDialog.ShowWindow(SW_RESTORE);
		bLiveEdges = true;
	}
	
	if (!bLiveCamera && !bLiveEdges && (m_timerID != NULL) ) 
	{
		KillTimer (m_timerID);
		m_timerID = NULL;
	}	
		
}

void CCameraTuneDlg::OnClose() 
{
	if (bLiveCamera)
	{
		CameraDialog.ShowWindow(SW_HIDE);
		bLiveCamera = false;
	}

	if (bLiveEdges)
	{
		EdgesDialog.ShowWindow(SW_HIDE);
		bLiveEdges = false;
	}

	// Timer Killing
	if (m_timerID != NULL)
		KillTimer (m_timerID);
	// Driver UnInit
	grabber.uninitialize();

	delete [] img_buffer;
	
	CDialog::OnClose();
}

void CCameraTuneDlg::OnStartCamera() 
{
	int ret;
	ret = grabber.initialize(options.boardN, options.sizeX, options.sizeY);
	if (ret==YARP_FAIL)
	{
		MessageBox("Video Camera init failed.", "Error.",MB_ICONERROR);
		OnClose();
	}
	else
	{
		MessageBox("Video Camera initialized correctly.", "Initilization done.",MB_ICONINFORMATION);
		GetDlgItem(IDC_SHOW_CAMERA)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOW_EDGES)->EnableWindow(TRUE);
		GetDlgItem(IDC_CALIBRATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_CAMERA)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTIONS)->EnableWindow(FALSE);
	}

}

void CCameraTuneDlg::OnTimer(UINT nIDEvent) 
{
	int ret;

	ret = grabber.waitOnNewFrame ();
	if (ret == YARP_FAIL)
	{
		MessageBox("Errors acquiring frames from videocamera.", "Error.",MB_ICONERROR);
		KillTimer (m_timerID);
		m_timerID = NULL;
	}
	else
	{
		unsigned char *buffer = NULL;
		ret = grabber.acquireBuffer(&buffer);
		if (ret == YARP_FAIL)
		{
			MessageBox("Errors acquiring frames from videocamera.", "Error.",MB_ICONERROR);
			KillTimer (m_timerID);
			m_timerID = NULL;
		}
		else 
		{
			memcpy((unsigned char *)img.GetRawBuffer(), buffer, options.sizeX * options.sizeY * 3);
			ret = grabber.releaseBuffer ();
			if (ret == YARP_FAIL)
			{
				MessageBox("Errors releasing videocamera buffer.", "Error.",MB_ICONERROR);
				KillTimer (m_timerID);
				m_timerID = NULL;
			}
			else 
			{
				YARPGenericImage *pImg = &img;
				if (bLiveCamera)
					CameraDialog.UpdateState(pImg);
				if (bLiveEdges)
					EdgesDialog.UpdateState(&img);
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CCameraTuneDlg::Calibrate()
{
	int ret;
	unsigned char *buffer = NULL;
	GetDlgItem(IDC_PROG_DESC)->SetWindowText("Grabbing frames (1st cycle)..");
	m_ProgBar.SetPos(0);
	for (int i=0; i<options.nImages; i++)
	{
		ret = grabber.waitOnNewFrame ();
		if (ret == YARP_FAIL)
		{
			MessageBox("Errors acquiring frames from videocamera.", "Error.",MB_ICONERROR);
			OnClose();
		}
		ret = grabber.acquireBuffer(&buffer);
		if (ret == YARP_FAIL)
		{
			MessageBox("Errors acquiring image buffer from videocamera.", "Error.",MB_ICONERROR);
			OnClose();
		}
		memcpy((unsigned char *)(img_buffer[i].GetRawBuffer()), buffer, options.sizeX * options.sizeY * 3);
		ret = grabber.releaseBuffer ();
		if (ret == YARP_FAIL)
		{
			MessageBox("Errors releasing videocamera buffer.", "Error.",MB_ICONERROR);
			OnClose();
		}
		m_ProgBar.SetPos(i+1);
	}
	
	GetDlgItem(IDC_PROG_DESC)->SetWindowText("Calculating main orientation (1st cycle)..");
	m_ProgBar.SetPos(0);
	double angle;
	double acc = 0.0;
	double initialOrientation;
	for (i=0; i<options.nImages; i++)
	{
			EdgesDialog.flt.apply(&(img_buffer[i]), &img_mono);
			hough_flt.apply(&img_mono);
			hough_flt.getMaxTheta(&angle);
			acc += _normalize0ToPI(PI/2.0-angle);
			m_ProgBar.SetPos(i+1);
	}
	initialOrientation = acc / options.nImages;

	// Display main orientation
	MessageBox("Now please move the eye and click on the OK button when the movement is complete.", "First cycle completed.",MB_OK);

	GetDlgItem(IDC_PROG_DESC)->SetWindowText("Grabbing frames (2nd cycle)..");
	m_ProgBar.SetPos(0);
	for (i=0; i<options.nImages; i++)
	{
		int ret;

		ret = grabber.waitOnNewFrame ();
		if (ret == YARP_FAIL)
		{
			MessageBox("Errors acquiring frames from videocamera.", "Error.",MB_ICONERROR);
			OnClose();
		}
		ret = grabber.acquireBuffer(&buffer);
		if (ret == YARP_FAIL)
		{
			MessageBox("Errors acquiring image buffer from videocamera.", "Error.",MB_ICONERROR);
			OnClose();
		}
		memcpy((unsigned char *)(img_buffer[i].GetRawBuffer()), buffer, options.sizeX * options.sizeY * 3);
		ret = grabber.releaseBuffer ();
		if (ret == YARP_FAIL)
		{
			MessageBox("Errors releasing videocamera buffer.", "Error.",MB_ICONERROR);
			OnClose();
		}
		m_ProgBar.SetPos(i+1);
	}

	GetDlgItem(IDC_PROG_DESC)->SetWindowText("Calculating main orientation (2nd cycle)..");
	m_ProgBar.SetPos(0);
	acc = 0.0;
	double finalOrientation;
	for (i=0; i<options.nImages; i++)
	{
			EdgesDialog.flt.apply(&(img_buffer[i]), &img_mono);
			hough_flt.apply(&img_mono);
			hough_flt.getMaxTheta(&angle);
			acc += _normalize0ToPI(PI/2.0-angle);
			m_ProgBar.SetPos(i+1);
	}
	finalOrientation = acc / options.nImages;
	CString Text;
	char Number[256];
	double difference = finalOrientation - initialOrientation;

	Text = "Calibration completed (using ";
	ACE_OS::sprintf(Number,"%d", options.nImages);
	Text += Number;
	Text += " images)\r\n";
	Text += "Initial Orientation is ";
	ACE_OS::sprintf(Number,"%.2f", initialOrientation);
	Text += Number;
	Text += "rad (";
	ACE_OS::sprintf(Number,"%.2f", _rad2deg(initialOrientation));
	Text += Number;
	Text += "deg).\r\n";
	Text += "Final Orientation is ";
	ACE_OS::sprintf(Number,"%.2f", finalOrientation);
	Text += Number;
	Text += "rad (";
	ACE_OS::sprintf(Number,"%.2f", _rad2deg(finalOrientation));
	Text += Number;
	Text += "deg).\r\n";
	Text += "Difference is ";
	ACE_OS::sprintf(Number,"%.2f", difference);
	Text += Number;
	Text += "rad (";
	ACE_OS::sprintf(Number,"%.2f", _rad2deg(difference));
	Text += Number;
	Text += "deg).\r\n";

	
	MessageDialog.initialOrientation = initialOrientation;
	MessageDialog.finalOrientation = finalOrientation;
	MessageDialog.m_Text = Text;
	if (options.autosave)
	{
		FILE *outFile;
		outFile = fopen(options.fileName,"a");
		
		if (!outFile)
		{
			MessageBox("Unable to autosave to selected file.", "Error.",MB_ICONERROR);
		}
		else
			fprintf(outFile, "%f;%f\n", initialOrientation, finalOrientation);

		fclose(outFile);
	}
	MessageDialog.DoModal();
}

double CCameraTuneDlg::_normalize0ToPI(double angle)
{
	int k;
	double normalized;
	if (abs(angle) > PI)
	{
		k = angle/PI;
		normalized = angle - double(k*PI);
	}
	else 
		normalized = angle;
	if (angle < 0)
		normalized += PI;
	
	return normalized;
}

double CCameraTuneDlg::_rad2deg(double rad)
{
	return double(rad*180.0/PI);
}

void CCameraTuneDlg::OnCalibrate() 
{
	// TODO: Add your control notification handler code here

	GetDlgItem(IDC_SHOW_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOW_EDGES)->EnableWindow(FALSE);
	GetDlgItem(IDC_CALIBRATE)->EnableWindow(FALSE);
	if (bLiveCamera)
	{
		CameraDialog.ShowWindow(SW_HIDE);
		bLiveCamera = false;
	}
	if (bLiveEdges)
	{
		EdgesDialog.ShowWindow(SW_HIDE);
		bLiveEdges = false;
	}
	if (m_timerID != NULL) 
	{
		KillTimer (m_timerID);
		m_timerID = NULL;
	}	
	Calibrate();
	GetDlgItem(IDC_SHOW_CAMERA)->EnableWindow(TRUE);
	GetDlgItem(IDC_SHOW_EDGES)->EnableWindow(TRUE);
	GetDlgItem(IDC_CALIBRATE)->EnableWindow(TRUE);
}

void CCameraTuneDlg::setupOptions()
{
	if (OptionsDialog.m_AutoSave == TRUE)
	{
		options.autosave = true;
	}
	else
	{
		options.autosave = false;
	}
	options.boardN = OptionsDialog.m_BoardN;
	ACE_OS::strcpy(options.fileName,(LPCSTR)(OptionsDialog.m_FileName) );
	options.nImages = OptionsDialog.m_BufferSize;
	options.refresh = OptionsDialog.m_RefreshTime;	
	options.rhoSize = OptionsDialog.m_Rho;
	options.thetaSize = OptionsDialog.m_Theta;
	options.sizeX = OptionsDialog.m_SizeX;
	options.sizeY = OptionsDialog.m_SizeY;
}

void CCameraTuneDlg::OnOptions() 
{
	BOOL ret;
	ret = OptionsDialog.DoModal();
	// controllare il valore di ritorno;
	if (ret)
		setupOptions();
	
}
