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
	CDialog( CBodyMapDlg::IDD, pParent ),
	_acquiringExamples( true ),
	_distanceMean( 0.0 ), _distanceStdv( 0.0 ), _distanceNumOfExamples(0)
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
	_Camera0Dialog.Create(CLiveCameraDlg::IDD, this);	_Camera0Dialog.SetWindowText("LIVE: Camera #0");
	_Camera1Dialog.Create(CLiveCameraDlg::IDD, this);	_Camera1Dialog.SetWindowText("LIVE: Camera #1");
	_Tracker0Dialog.Create(CLiveTrackerDlg::IDD, this); _Tracker0Dialog.SetWindowText("LIVE: Tracker #0");
	_Tracker1Dialog.Create(CLiveTrackerDlg::IDD, this); _Tracker1Dialog.SetWindowText("LIVE: Tracker #1");
	_GloveDialog.Create(CLiveGloveDlg::IDD, this);
	// register and connect ports
	if ( RegisterAndConnectPorts() == YARP_FAIL ) {
		MessageBox("Could not register or connect ports.", "Fatal error.", MB_ICONERROR);
		exit(YARP_FAIL);
	}
	// show windows
	_Camera0Dialog.ShowWindow(SW_SHOW);
	_Camera1Dialog.ShowWindow(SW_SHOW);
	_Tracker0Dialog.ShowWindow(SW_SHOW);

	// create learning machine
	string name = "prova";
	_learner = new SVMLearningMachine(3,4,50,name);
//	set tolerance at one cm (0.4 times an inch)
//	double tolerances[3] = { 0.4, 0.4, 0.4 };
//	_learner = new UniformSVMLearningMachine(3,4,250,"prova",tolerances);

	// try and load a previous model; if found, do not acquire enything else
	if ( _learner->load() ) {
		_acquiringExamples = false;
	}

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

void CBodyMapDlg::OnLiveTracker() 
{

	// toggle dialogs
	_Tracker0Dialog.ShowWindow(!_Tracker0Dialog.ShowWindow(SW_SHOWNA));
	
}

void CBodyMapDlg::OnLiveGlove() 
{

	// toggle dialog
	_GloveDialog.ShowWindow(!_GloveDialog.ShowWindow(SW_SHOWNA));

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
		if ( _acquiringExamples == false ) {
			GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
		}
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
	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);

}

void CBodyMapDlg::OnClose() 
{

	if ( GetDlgItem(IDC_DISCONNECT)->IsWindowEnabled() ) {
		OnDisconnect();
	}
	DisconnectAndUnregisterPorts();
	delete _learner;
	CDialog::OnClose();

}

void CBodyMapDlg::OnTimer(UINT nIDEvent)
{

	// get and read data off the collector
	_settings._cmd_outport.Content() = CCmdGetData;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdFailed ) {
		MessageBox("Could not read data from mirrorCollector.", "Error.",MB_ICONERROR);
	}
	_settings._data_inport.Read();
	_settings._data = _settings._data_inport.Content();
	_settings._img0_inport.Read();
	_settings._img0.Refer(_settings._img0_inport.Content());
	_settings._img1_inport.Read();
	_settings._img1.Refer(_settings._img1_inport.Content());

	// (1) ---------- find real tracker position and show it
	int x0, y0, x1, y1;
	FindTrackerXY(_settings._img0, &x0, &y0); ShowTrackerXY(_settings._img0, x0, y0);
	FindTrackerXY(_settings._img1, &x1, &y1); ShowTrackerXY(_settings._img1, x1, y1);

	// (2) ---------- predict and show expected position
	double x[3];
	x[0] = _settings._data.tracker0Data.x;
	x[1] = _settings._data.tracker0Data.y;
	x[2] = _settings._data.tracker0Data.z;
	double predicted_y[4];
	_learner->predictValue(x,predicted_y);
	ShowExpectedTrackerXY(_settings._img0, (int)predicted_y[0], (int)predicted_y[1]);
	ShowExpectedTrackerXY(_settings._img1, (int)predicted_y[2], (int)predicted_y[3]);

	// (3) ------- if it is the case, add the new sample and re-train
	double y[4];
	y[0] = x0; y[1] = y0; y[2] = x1; y[3] = y1;
	if ( _acquiringExamples ) {
		if ( _learner->addExample(x,y) ) {
			// if we haven't yet filled the samples pool, show we got one more
			char title[50];
			ACE_OS::sprintf(title, "%d/%d",	_learner->getExampleCount(),
				_learner->getNumOfExamples());
			GetDlgItem(IDC_ACQ_START)->SetWindowText(title);
			// and, every 20 new samples after the first 5, re-train
			if ( _learner->getExampleCount() % 20 == 5 ) {
				_learner->train();
			}
		} else {
			// otherwise, train for the last time, save the model and stop acquiring
			_learner->train();
			GetDlgItem(IDC_ACQ_START)->SetWindowText("Reset");
			GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
			_learner->save();
			_acquiringExamples = false;
		}
	}

	// (4) ------- measure the error and display its stats
	double newDistance = Distance(y,predicted_y,4);
	_distanceMean = (1/(double)(_distanceNumOfExamples+1)) *
				    ( _distanceNumOfExamples*_distanceMean + newDistance );
	_distanceStdv = sqrt( (1/(double)(_distanceNumOfExamples+1)) *
					      ( _distanceNumOfExamples*_distanceStdv*_distanceStdv + 
					        (newDistance-_distanceMean)*(newDistance-_distanceMean)
					    ) );
	char title[50];
	ACE_OS::sprintf(title, "%.2lf %.2lf", _distanceMean, _distanceStdv);
	GetDlgItem(IDC_DISTANCE)->SetWindowText(title);
	++_distanceNumOfExamples;

	// update dialogs
	_Tracker0Dialog.UpdateState(_settings._data.tracker0Data);
	_Tracker1Dialog.UpdateState(_settings._data.tracker1Data);
//	_GloveDialog.UpdateState(_settings._data.gloveData, _settings._data.pressureData, _settings._data.GTData);
	_Camera0Dialog.UpdateState(&_settings._img0);
	_Camera1Dialog.UpdateState(&_settings._img1);

	CDialog::OnTimer(nIDEvent);

}

void CBodyMapDlg::OnAcqStart()
{

	// reset learning machine
	_learner->reset();
	
	// reset distance stats
	_distanceNumOfExamples = 0;
	_distanceMean = 0.0;
	_distanceStdv = 0.0;

	// and acquire samples again!
	_acquiringExamples = true;

	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);

}

// ---------------- procedures which are not associated with MFC objects

const double CBodyMapDlg::Distance( const double x[], const double y[], const unsigned int dim ) const
{

	double distSquared = 0.0;

	for ( int i=0; i<dim; ++i ) {
		distSquared += (x[i]-y[i])*(x[i]-y[i]);
	}

	return sqrt(distSquared);

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
