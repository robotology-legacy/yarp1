// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  main.cpp
// 
//     Description:  
// 
//         Version:  $Id: main.cpp,v 1.3 2004-03-03 10:19:14 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>

#include <YARPImages.h>
#include <YARPLogpolar.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPConfigFile.h>

#if defined(__QNXEurobot__)

#	include <YARPEurobotSoundGrabber.h>
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__WIN32Babybot__)

#	include <YARPBabybotSoundGrabber.h>
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__QNXBabybot__)

#	include <YARPBabybotSoundGrabber.h>
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__LinuxTest__)
/// apparently small difference in macro subst, need to investigate, weird.
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>(x)(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#else

#error "pls specify a setup by defining symbol, see code above"

#endif

//----------------------------------------------------------------------
// Global parameters  
//----------------------------------------------------------------------
char	_name[512];
char	_fgdataname[512];
char	_netname[512];
bool	_client		= false;
bool	_simu		= false;
bool	_fgnetdata	= false;
int		_board_no	= 0;
char    __filename[256] = "sound.ini";

extern int __debug_level;

// ===  FUNCTION  ======================================================================
// 
//         Name:  ParseParams
// 
//  Description:  This function parse the parameters passed to the grabber
// 
//    Author:  Ing. Carlos Beltran
//  Revision:  none
// =====================================================================================
int 
ParseParams (int argc, char *argv[]) 
{
	int i;
	
	ACE_OS::sprintf (_name, "/%s/o:sound", argv[0]);
	ACE_OS::sprintf (_fgdataname,"/%s/i:fgdata", argv[0]);
	ACE_OS::sprintf (_netname, "default");

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+') {
			ACE_OS::sprintf (_name, "/%s/o:sound", argv[i]+1);
			ACE_OS::sprintf (_fgdataname,"/%s/i:fgdata", argv[i]+1);
		}
		else
		if (argv[i][0] == '-') {
			switch (argv[i][1])
			{
			case 'w':
				break;

			case 'h':
				break;

			case 'b':
				_board_no = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 't':
				ACE_OS::fprintf (stdout, "soundgrabber acting as a receiver client...\n");
				ACE_OS::sprintf (_name, "%s", argv[i+1]);
				i++;
				_client = true;
				_simu = false;
				break;

			case 's':
				ACE_OS::fprintf (stdout, "simulating a soundgrabber...\n");
				_simu = true;
				_client = false;
				break;

			case 'n':
				ACE_OS::fprintf (stdout, "sending to network : %s\n", argv[i+1]);
				ACE_OS::sprintf (_netname, "%s", argv[i+1]);
				i++;
				break;

			case 'f':
				ACE_OS::fprintf(stdout, "grabber receiving data from network mode...\n");
				_fgnetdata = true;
				break;

			default:
				ACE_OS::fprintf(stdout, "The %s option is not supported",argv[i]);
				break;
			}
		}
		else {
			ACE_OS::fprintf (stderr, "unrecognized parameter %d:%s\n", i, argv[i]);
		}
	}

	return YARP_OK; 
}

// =====================================================================================
//        Class:  FgNetDataPort
// 
//  Description:  This class extends a YARPPort in order to implement the OnRead callback function
//  It receives a bottle with information to adjust the sound 
//  To be used to fgadjuster photon application. 
// 
//       Author:  Ing. Carlos Beltran
//      Created:  15/01/2003 10:36:00 W. Europe Standard Time
//     Revision:  NOTE: Modifications should be done to support sound variables
//     (volume, mute...etc) 
// =====================================================================================

#if !defined(__LinuxTest__)
class FgNetDataPort : public YARPInputPortOf<YARPBottle>
{
	protected:
		YARPBottle m_bottle;
		YARPSoundGrabber * m_gb;

	public:
		FgNetDataPort (YARPSoundGrabber * gb):YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS, YARP_UDP) 
		{ 
			m_gb = gb;
			m_bottle.reset();
		}

		virtual void OnRead(void);
};

//--------------------------------------------------------------------------------------
//       Class:  FgNetDataPort
//      Method:  OnRead()
// Description:  This is just the the callback funtion that reads the bottle and sets
// the adequate driver functions
//--------------------------------------------------------------------------------------
void FgNetDataPort::OnRead(void)
{
	Read ();
	m_bottle = Content();
	m_bottle.rewind();
	//m_bottle.readInt((int *)&m_dithf);
	
	//m_gb->setDithFrame(m_dithf);
}
#endif

// =====================================================================================
//        Class:  mainthread
// 
//  Description:  
// 
//       Author:  Ing. Carlos Beltran
//      Created:  25/02/2004 10:30:43 W. Europe Standard Time
//     Revision:  none
// =====================================================================================
class mainthread : public YARPThread
{
public:
	virtual void Body (void)
	{
		if (_client)
		{
			_runAsClient ();
			return;
		}
		else
		if (_simu)
		{
			_runAsSimulation ();
			return;
		}
#if !defined(__LinuxTest__)
		else
		{
			_runAsNormally ();
			return;
		}
#endif
	}

	int _runAsClient(void);
	int _runAsSimulation (void);
	int _runAsNormally (void);
};


//--------------------------------------------------------------------------------------
//       Class:  mainthread
//      Method:  _runAsClient
// Description:  This method acts as client of other soundgrabber
//--------------------------------------------------------------------------------------
int 
mainthread::_runAsClient (void)
{
	//YARPImageOf<YarpPixelBGR> img;
	YARPInputPortOf<YARPGenericImage> inport(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	inport.Register (_name, _netname);
	int frame_no = 0;

	char savename[512];
	memset (savename, 0, 512);

	while (!IsTerminated())
	{
		inport.Read ();
		//img.CastCopy (inport.Content());

		frame_no++;
		ACE_OS::printf (">>> got a frame %d\n", frame_no);

		ACE_OS::sprintf (savename, "./soundgrab_test%04d.wav", frame_no);
		//YARPImageFile::Write (savename, img);
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}


//--------------------------------------------------------------------------------------
//       Class:  mainthread
//      Method:  _runAsSimulation 
// Description:  This function simulates a recording and sends the simulated data into
// the network. It is used to test this level plus clients and network conections
//--------------------------------------------------------------------------------------
int 
mainthread::_runAsSimulation (void)
{
	YARPImageOf<YarpPixelBGR> img;
	DeclareOutport(outport);

	outport.Register (_name, _netname);
	int frame_no = 0;
	ACE_OS::fprintf (stdout, "starting up simulation of a soundgrabber...\n");

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!IsTerminated())
	{
		YARPTime::DelayInSeconds (0.04);
	
		//img.Zero ();
		//*(img.GetRawBuffer() + (frame_no % img.GetAllocatedDataSize())) = -1;

		outport.Content().Refer (img);
		outport.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			// ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}


//--------------------------------------------------------------------------------------
//       Class:  mainthread
//      Method:  _runAsNormally
// Description:  This method normally instanciates an access to the low level driver.
// It gets the sound data and sends it to the network
//--------------------------------------------------------------------------------------
int 
mainthread::_runAsNormally (void)
{
	int _Channels = 0;
	int _SamplesPerSec= 0;
	int _BitsPerSample = 0;
	int _BufferLength = 0;

	HMMIO hmmio;
	MMCKINFO ckRIFF;
	MMCKINFO ck;
	WAVEFORMATEX waveFormat;

	unsigned char *buffer = NULL;
	int frame_no = 0;

	YARPSoundGrabber soundgrabber;
	YARPImageOf<YarpPixelBGR> img;

	DeclareOutport(outport);
	outport.Register (_name, _netname);

	//----------------------------------------------------------------------
	//  Get sound information from ini file
	//----------------------------------------------------------------------
	YARPConfigFile file;
	char *root = GetYarpRoot();
	char path[256];

#if defined(__QNXEurobot__)
	ACE_OS::sprintf (path, "%s/conf/eurobot/\0", root); 
#else
	ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
#endif

	file.set(path, __filename);
	file.get("[GENERAL]", "Channels", &_Channels, 1);
	file.get("[GENERAL]", "SamplesPerSec", &_SamplesPerSec, 1);
	file.get("[GENERAL]", "BitsPerSample", &_BitsPerSample, 1);
	file.get("[GENERAL]", "BufferLength", &_BufferLength, 1);

	//----------------------------------------------------------------------
	//  Initialize the grabber
	//----------------------------------------------------------------------
	soundgrabber.initialize (_board_no);
	
	//----------------------------------------------------------------------
	//  Start the data reception port in the case the option is active
	//----------------------------------------------------------------------
	FgNetDataPort  * m_fg_net_data;
	if (_fgnetdata)
	{
		m_fg_net_data = new FgNetDataPort(&soundgrabber);
		m_fg_net_data->Register (_fgdataname,_netname);
	} 

	ACE_OS::fprintf (stdout, "starting up soundgrabber...\n");

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	//----------------------------------------------------------------------
	//  Initialize the WAVEFORMATEX ini sound file data
	//----------------------------------------------------------------------
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = _Channels;
	waveFormat.nSamplesPerSec = _SamplesPerSec;
	waveFormat.wBitsPerSample = _BitsPerSample;
	waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample/8);
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;
	//----------------------------------------------------------------------
	//  Open and initialize a WAVE file
	//----------------------------------------------------------------------
	hmmio = mmioOpen("dest.wav",
					 NULL,
					 MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE |
					 MMIO_ALLOCBUF);

	ckRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	ckRIFF.cksize  = 0;
	mmioCreateChunk(hmmio, &ckRIFF, MMIO_CREATERIFF);

	ck.ckid   = mmioFOURCC('f', 'm', 't', ' ');
	ck.cksize = 0L;
	mmioCreateChunk(hmmio, &ck, 0);

	mmioWrite(hmmio, (HPSTR)&waveFormat, sizeof(WAVEFORMATEX));
	mmioAscend(hmmio, &ck, 0);

	ck.ckid   = mmioFOURCC('d', 'a', 't', 'a');
	ck.cksize = 0;
	mmioCreateChunk(hmmio, &ck, 0);	

	//----------------------------------------------------------------------
	// Main loop 
	//----------------------------------------------------------------------
	while (!IsTerminated())
	{
		soundgrabber.waitOnNewFrame ();
		soundgrabber.acquireBuffer(&buffer);

		//memcpy((unsigned char *)img.GetRawBuffer(), buffer, 16);
		/// sends the buffer. Dont know if it is efficient like this or I should
		// memcpy the stuff to a local buffer
		////Here I should refer the correct data type. I still have to decide this type
		///outport.Content().Refer (buffer); //is this correct? do I pass the pointer to the buffer?
		//outport.Write();

		mmioWrite(hmmio, 
				  (const char *)buffer,
				  _BufferLength);

		soundgrabber.releaseBuffer ();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f soundframes #%d acquired\r", (cur-start)/250, frame_no);
			// ACE_OS::fprintf (stdout, "frame number %d acquired\r", frame_no);
			start = cur;
		}
	}
	
	mmioAscend(hmmio, &ck, 0);
	mmioAscend(hmmio, &ckRIFF, 0);
	mmioClose(hmmio, 0);

	//destroy fg_net_data port
	if (_fgnetdata)
	{
		if (m_fg_net_data != NULL)
			delete m_fg_net_data; //can this be done better? (closind the port?)
	}

	soundgrabber.uninitialize ();

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

// ===  FUNCTION  ======================================================================
// 
//         Name:  main
// 
//  Description:  Obviously the main function. 
// 
//    Author:  Ing. Carlos Beltran
//  Revision:  none
// =====================================================================================
int 
main (int argc, char *argv[])
{
	///__debug_level = 80;

	YARPScheduler::setHighResScheduling ();

	ParseParams (argc, argv);

	mainthread _thread;
	_thread.Begin();

	char c = 0;

	do
	{
		cout << "Type q+return to quit" << endl;
		cin >> c;
	}
	while (c != 'q');

	_thread.End(-1);

	return YARP_OK;
}
