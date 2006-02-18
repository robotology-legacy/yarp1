// BodyMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BodyMap.h"
#include "BodyMapDlg.h"

#include <yarp/YARPImages.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBodyMapDlg dialog

CBodyMapDlg::CBodyMapDlg(CWnd* pParent /*=NULL*/) :
   CDialog( CBodyMapDlg::IDD, pParent ), _acquiringSamples( false )
{

	//{{AFX_DATA_INIT(CBodyMapDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

}

void CBodyMapDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBodyMapDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CBodyMapDlg, CDialog)
	//{{AFX_MSG_MAP(CBodyMapDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LIVE_CAMERA, OnLiveCamera)
	ON_BN_CLICKED(IDC_LIVE_GLOVE, OnLiveGlove)
	ON_BN_CLICKED(IDC_LIVE_TRACKER, OnLiveTracker)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ACQ_START, OnAcqStart)
	ON_BN_CLICKED(IDC_ACQ_STOP, OnAcqStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBodyMapDlg message handlers

BOOL CBodyMapDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) {
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// create and set up dialogs
	_GloveDialog.Create(CLiveGloveDlg::IDD, this);
	_Tracker0Dialog.Create(CLiveTrackerDlg::IDD, this);
	_Tracker0Dialog.SetWindowText("LIVE: Tracker #0");
	_Tracker1Dialog.Create(CLiveTrackerDlg::IDD, this);
	_Tracker1Dialog.SetWindowText("LIVE: Tracker #1");
	_Camera0Dialog.Create(CLiveCameraDlg::IDD, this);
	_Camera0Dialog.SetWindowText("LIVE: Camera #0");
	_Camera1Dialog.Create(CLiveCameraDlg::IDD, this);
	_Camera1Dialog.SetWindowText("LIVE: Camera #1");
	// enable windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	// register and connect ports
	if ( RegisterAndConnectPorts() == YARP_FAIL ) {
		MessageBox("Could not register or connect ports.", "Fatal error.", MB_ICONERROR);
		exit(YARP_FAIL);
	}
	// show windows
	_Camera0Dialog.ShowWindow(SW_SHOW);
	_Camera1Dialog.ShowWindow(SW_SHOW);
	_Tracker0Dialog.ShowWindow(SW_SHOW);

	_learningBlock = new BodyMapLearningBlock(3,4,50);

	return TRUE;

}

void CBodyMapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);

}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CBodyMapDlg::OnPaint() 
{

	if (IsIconic()) {
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
	} else {
		CDialog::OnPaint();
	}

}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBodyMapDlg::OnQueryDragIcon()
{

	return (HCURSOR) m_hIcon;

}

void CBodyMapDlg::OnLiveCamera() 
{

	// toggle dialogs
	_Camera0Dialog.ShowWindow(!_Camera0Dialog.ShowWindow(SW_SHOWNA));
	_Camera1Dialog.ShowWindow(!_Camera1Dialog.ShowWindow(SW_SHOWNA));

}

void CBodyMapDlg::OnLiveGlove() 
{

	// toggle dialog
	_GloveDialog.ShowWindow(!_GloveDialog.ShowWindow(SW_SHOWNA));

}

void CBodyMapDlg::OnLiveTracker() 
{

	// toggle dialogs
	_Tracker0Dialog.ShowWindow(!_Tracker0Dialog.ShowWindow(SW_SHOWNA));
	
}

void CBodyMapDlg::OnConnect() 
{

	// send connect command to collector
	_settings._cmd_outport.Content() = CCmdConnect;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdSucceeded ) {
		_settings._cmd_inport.Read();
		int tmpOptions = _settings._cmd_inport.Content();
		// must be using both cameras and tracker 0
		if ( tmpOptions != (HardwareUseTracker0|HardwareUseCamera0|HardwareUseCamera1) ) {
			MessageBox("Must use both cameras and tracker 0 only.", "Error.",MB_ICONERROR);
			return;
		}
		// read images dimensions
		_settings._cmd_inport.Read();
		_options.sizeX = _settings._cmd_inport.Content();
		_settings._cmd_inport.Read();
		_options.sizeY = _settings._cmd_inport.Content();
		_settings._img0.Resize (_options.sizeX, _options.sizeY);
		_settings._img1.Resize (_options.sizeX, _options.sizeY);
		_Camera0Dialog.pImage = &_settings._img0;
		_Camera0Dialog.sizeX = _options.sizeX;
		_Camera0Dialog.sizeY = _options.sizeY;
		_Camera1Dialog.pImage = &_settings._img1;
		_Camera1Dialog.sizeX = _options.sizeX;
		_Camera1Dialog.sizeY = _options.sizeY;
		// enable the required windows
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
		GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
		// start live timer
		_settings._timerID = SetTimer(1, _options.refreshFrequency, NULL);
		_ASSERT (_settings._timerID != 0);	
	} else {
		MessageBox("Could not connect to mirrorCollector.", "Error.",MB_ICONERROR);
	}	

}

void CBodyMapDlg::OnDisconnect() 
{

	// disconnect collector
	_settings._cmd_outport.Content() = CCmdDisconnect;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdFailed) {
		MessageBox("Could not disconnect from mirrorCollector.", "Error.",MB_ICONERROR);
	}

	// kill timer
	KillTimer(_settings._timerID);

	// enable/disable required windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);

}

void CBodyMapDlg::OnClose() 
{

	if ( GetDlgItem(IDC_DISCONNECT)->IsWindowEnabled() ) {
		OnDisconnect();
	}
	DisconnectAndUnregisterPorts();
	delete _learningBlock;
	CDialog::OnClose();

}

void CBodyMapDlg::OnTimer(UINT nIDEvent)
{

	// this is the timer event. each time the timer ticks, we gather data
	// off the collector and update the live show. Moreover, if acquiringSamples
	// is true we store one more sample in the learning block.

	double x[3], y[4];

	// get data
	_settings._cmd_outport.Content() = CCmdGetData;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdSucceeded ) {
		// read data (tracker and images)
		_settings._data_inport.Read();
		_settings._data = _settings._data_inport.Content();
		_settings._img0_inport.Read();
		_settings._img0.Refer(_settings._img0_inport.Content());
		_settings._img1_inport.Read();
		_settings._img1.Refer(_settings._img1_inport.Content());
		// predict and show expected position
		x[0] = _settings._data.tracker0Data.x;
		x[1] = _settings._data.tracker0Data.y;
		x[2] = _settings._data.tracker0Data.z;
		_learningBlock->predict(x,y);
		ShowExpectedTrackerXY(_settings._img0, (int)y[0], (int)y[1]);
		ShowExpectedTrackerXY(_settings._img1, (int)y[2], (int)y[3]);
		// find real tracker position and show it
		int x0, y0, x1, y1;
		FindTrackerXY(_settings._img0, &x0, &y0);
		ShowTrackerXY(_settings._img0, x0, y0);
		FindTrackerXY(_settings._img1, &x1, &y1);
		ShowTrackerXY(_settings._img1, x1, y1);
		// if we are gathering samples, store this one
		if ( _acquiringSamples ) {
			y[0] = x0;
			y[1] = y0;
			y[2] = x1;
			y[3] = y1;
			int addedOk = _learningBlock->addSample(x, y);
			if ( addedOk ) {
				char title[50];
				ACE_OS::sprintf(title, "Acquired sample %d (%.2lf%%)", _learningBlock->getSampleCount(),
					(double)_learningBlock->getSampleCount()*100.0/(double)_learningBlock->getNumOfSamples());
				AfxGetMainWnd()->SetWindowText(title);
			} else {
				_acquiringSamples = false;
				AfxGetMainWnd()->SetWindowText("Training...");
				_learningBlock->train();
				AfxGetMainWnd()->SetWindowText("BodyMap");
				GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
			}
		}
	} else {
		MessageBox("Could not read data from mirrorCollector.", "Error.",MB_ICONERROR);
	}

	// update dialogs
	_Tracker0Dialog.UpdateState(_settings._data.tracker0Data);
	_Tracker1Dialog.UpdateState(_settings._data.tracker1Data);
	_GloveDialog.UpdateState(_settings._data.gloveData, _settings._data.pressureData, _settings._data.GTData);
	_Camera0Dialog.UpdateState(&_settings._img0);
	_Camera1Dialog.UpdateState(&_settings._img1);

	CDialog::OnTimer(nIDEvent);

}

void CBodyMapDlg::OnAcqStart()
{

	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	_acquiringSamples = true;

}

void CBodyMapDlg::OnAcqStop() {}

// ---------------- procedures which are not associated with MFC objects

int CBodyMapDlg::RegisterAndConnectPorts()
{

	char tmpBMPortName[255];
	char tmpMCPortName[255];

	// ----------- data
	// set up collector's port name
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:str", _settings.MirrorCollectorPortName);
	// first try and disconnect collector's port - useful if previous instance of BodyMap has crashed
	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:str", _settings.BodyMapPortName);
	_settings._data_inport.Connect(tmpMCPortName, tmpBMPortName);
	// now register BodyMap's port
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:str", _settings.BodyMapPortName);
	if ( _settings._data_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	// then try and connect to collector's port
	if ( _settings._data_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	// ----------- images
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img0", _settings.MirrorCollectorPortName);
	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:img0", _settings.BodyMapPortName);
	_settings._img0_inport.Connect(tmpMCPortName, tmpBMPortName);
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:img0", _settings.BodyMapPortName);
	if ( _settings._img0_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img0", _settings.MirrorCollectorPortName);
	if ( _settings._img0_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img1", _settings.MirrorCollectorPortName);
	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:img1", _settings.BodyMapPortName);
	_settings._img1_inport.Connect(tmpMCPortName, tmpBMPortName);
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:img1", _settings.BodyMapPortName);
	if ( _settings._img1_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	if ( _settings._img1_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	// ----------- commands
	ACE_OS::sprintf(tmpBMPortName,"/%s/o:int", _settings.BodyMapPortName);
	if ( _settings._cmd_outport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/i:int", _settings.MirrorCollectorPortName);
	if ( _settings._cmd_outport.Connect(tmpBMPortName, tmpMCPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:int", _settings.MirrorCollectorPortName);
	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:int", _settings.BodyMapPortName);
	_settings._cmd_inport.Connect(tmpMCPortName, tmpBMPortName);
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:int", _settings.BodyMapPortName);
	if ( _settings._cmd_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	if ( _settings._cmd_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}

	return YARP_OK;

}

void CBodyMapDlg::DisconnectAndUnregisterPorts()
{

	char tmpBMPortName[255];
	char tmpMCPortName[255];

	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:int", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:int", _settings.MirrorCollectorPortName);
	_settings._cmd_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._cmd_inport.Unregister();

	ACE_OS::sprintf(tmpBMPortName,"/%s/o:int", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"!/%s/i:int", _settings.MirrorCollectorPortName);
	_settings._cmd_outport.Connect(tmpBMPortName, tmpMCPortName);
	_settings._cmd_outport.Unregister();
	
	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:img1", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img1", _settings.MirrorCollectorPortName);
	_settings._img1_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._img1_inport.Unregister();

	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:img0", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img0", _settings.MirrorCollectorPortName);
	_settings._img0_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._img0_inport.Unregister();

	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:str", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:str", _settings.MirrorCollectorPortName);
	_settings._data_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._data_inport.Unregister();

}

void CBodyMapDlg::ShowTrackerXY(YARPImageOf<YarpPixelBGR>& img, int x, int y)
{

	// draw a white cross where the tracker is actually seen

	YarpPixelBGR tmpBGRWhitePixel(255,255,255);

	if ( x>0 && y>0 ) {
		YARPSimpleOperations::DrawCross<YarpPixelBGR>(img, x, y, tmpBGRWhitePixel, 5);
	}

}

void CBodyMapDlg::ShowExpectedTrackerXY(YARPImageOf<YarpPixelBGR>& img, int x, int y)
{

	// draw a YELLOW cross where the system thinks it is
	
	YarpPixelBGR tmpBGRYellowPixel(255,255,0);

	YARPSimpleOperations::DrawCross<YarpPixelBGR>(img, x, y, tmpBGRYellowPixel, 10);

}

void CBodyMapDlg::FindTrackerXY(YARPImageOf<YarpPixelBGR>& img, int* x, int* y)
{

	unsigned long sumX = 0, sumY = 0, nOfPixels = 0;

	IMGFOR(img,i,j) {
		
		double r = img(i,j).r/255.0;
		double g = img(i,j).g/255.0;
		double b = img(i,j).b/255.0;
		double h, s, v;
		double max = (r>=b && r>=g) ? r : ((g>=r && g>=b) ? g : b );
		double min = (r<=b && r<=g) ? r : ((g<=r && g<=b) ? g : b );

		if ( max == min ) {
			h = 0;
		} else {
			if ( max == r ) h = 60*(g-b)/(max-min)+0;
			if ( max == g ) h = 60*(b-r)/(max-min)+120;
			if ( max == b ) h = 60*(r-g)/(max-min)+240;
		}
		if ( max == 0 ) {
			s = 0;
		} else {
			s = (max-min)/max;
		}
		v = max;

		if ( cos(h/360*2*M_PI)>0.85 && s>0.5 && v>0.65 ) {
			sumX += i;
			sumY += j;
			++nOfPixels;
		}
	}

	if ( nOfPixels > 0 ) {
		*x = (int)(sumX/nOfPixels);
		*y = (int)(sumY/nOfPixels);
	} else {
		*x = -1;
		*y = -1;
	}

}
