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
//         Version:  $Id: main.cpp,v 1.8 2004-04-27 16:17:48 beltran Exp $
// 
//          Author:  Eng. Carlos Beltran (Carlos), cbeltran@dist.unige.it
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
#include <YARPSound.h>
#include <YARPSoundPortContent.h>
#include <YARPLogpolar.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPConfigFile.h>

#if defined(__QNXEurobot__)

#	include <YARPEurobotSoundGrabber.h>
#	define DeclareOutport(x) YARPOutputPortOf<YARPSoundBuffer>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__WIN32Babybot__)

#	include <YARPBabybotSoundGrabber.h>
#	define DeclareOutport(x) YARPOutputPortOf<YARPSoundBuffer>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__QNXBabybot__)

#	include <YARPBabybotSoundGrabber.h>
#	define DeclareOutport(x) YARPOutputPortOf<YARPSoundBuffer>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__LinuxTest__)
/// apparently small difference in macro subst, need to investigate, weird.
#	define DeclareOutport(x) YARPOutputPortOf<YARPSoundBuffer>(x)(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#else

#error "pls specify a setup by defining symbol, see code above"

#endif

//----------------------------------------------------------------------
// Global parameters  
//----------------------------------------------------------------------
char _name[512];
char _fgdataname[512];
char _netname[512];
bool _client          = false;
bool _simu            = false;
bool _fgnetdata       = false;
int  _board_no        = 0;
char __filename[256]  = "sound.ini";

//----------------------------------------------------------------------
//  Default Sound paramters
//----------------------------------------------------------------------
int _Channels      = 2;
int _SamplesPerSec = 44100;
int _BitsPerSample = 16;
int _BufferLength  = 8192;
int _volume        = 50;

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
ParseParams (int argc, char *argv[], int visualize = 0) 
{
	int i;
	
	ACE_OS::sprintf (_name      , "/%s/o:sound" , argv[0]);
	ACE_OS::sprintf (_fgdataname, "/%s/i:fgdata", argv[0]);
	ACE_OS::sprintf (_netname   , "default");

	if (visualize)
	{
		ACE_OS::fprintf(stdout, "USE: soundgrab <+name -Parameters>\n");
		ACE_OS::fprintf(stdout, "c -> Channels\n");
		ACE_OS::fprintf(stdout, "a -> SamplesPerSecond\n");
		ACE_OS::fprintf(stdout, "i -> BitsPerSample\n");
		ACE_OS::fprintf(stdout, "l -> BufferLength\n");
		ACE_OS::fprintf(stdout, "b -> BoardNumber\n");
		ACE_OS::fprintf(stdout, "t -> Receiver client modality\n");
		ACE_OS::fprintf(stdout, "s -> Server Simulation\n");
		ACE_OS::fprintf(stdout, "n -> Network\n");
		ACE_OS::fprintf(stdout, "f -> Open port for volume/gain/mute data\n");

		return YARP_OK;
   	}

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '?') { 
			ParseParams(argc, argv, 1);
			exit(0);
		}
		else
		if (argv[i][0] == '+') 
		{
			ACE_OS::sprintf (_name      , "/%s/o:sound" , argv[i]+1);
			ACE_OS::sprintf (_fgdataname, "/%s/i:fgdata", argv[i]+1);
		}
		else
		if (argv[i][0] == '-') 
		{
			switch (argv[i][1])
			{
            case 'c':                                     // Channels
                _Channels = ACE_OS::atoi(argv[i+1]);
                i++;
                break;

            case 'a':                                     // SamplesPerSecond
                _SamplesPerSec = ACE_OS::atoi(argv[i+1]);
                i++;
                break;

            case 'i':                                     // BitsPerSample
                _BitsPerSample = ACE_OS::atoi(argv[i+1]);
                i++;
                break;

            case 'l':                                     // BufferLength
                _BufferLength = ACE_OS::atoi(argv[i+1]);
                i++;
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
		else 
		{
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
//       Author:  Eng. Carlos Beltran
//      Created:  15/01/2003 10:36:00 
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
	/*************************************************************
	 * Here the reading of adjusting data must be done.          *
	 * The bottle object is used to send the data in the network *
	 *************************************************************/
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
//       Author:  Eng. Carlos Beltran
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
// Description:  This method acts as client of other soundgrabber. Simply reads the sound
// buffer from the network and puts it in a WAVE file
//--------------------------------------------------------------------------------------
int 
mainthread::_runAsClient (void)
{
	YARPSoundBuffer buffer;
	YARPInputPortOf<YARPSoundBuffer> inport(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	HMMIO        hmmio;
	MMCKINFO     ckRIFF;
	MMCKINFO     ck;
	WAVEFORMATEX waveFormat;

	inport.Register (_name, _netname);
	int frame_no = 0;

	char savename[512];
	memset (savename, 0, 512);
	ACE_OS::sprintf (savename, "./soundgrab_test.wav");

	//----------------------------------------------------------------------
	//  Initialize the WAVEFORMATEX ini sound file data
	//----------------------------------------------------------------------
	waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
	waveFormat.nChannels       = _Channels;
	waveFormat.nSamplesPerSec  = _SamplesPerSec;
	waveFormat.wBitsPerSample  = _BitsPerSample;
	waveFormat.nBlockAlign     = waveFormat.nChannels * (waveFormat.wBitsPerSample/8);
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize          = 0;

	//----------------------------------------------------------------------
	//  Open and initialize a WAVE file
	//  Note: I am using mmio functions. I have to figure out how to do
	//  this in QNX and Linux
	//----------------------------------------------------------------------
	hmmio = mmioOpen(savename,
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

	while (!IsTerminated())
	{
		inport.Read ();
		buffer.Refer (inport.Content());

		mmioWrite(hmmio, 
				  (const char *)buffer.GetRawBuffer(),
				  _BufferLength);

		frame_no++;
	}

	//----------------------------------------------------------------------
	//  Close the WAVE file
	//----------------------------------------------------------------------
	mmioAscend(hmmio, &ck, 0);
	mmioAscend(hmmio, &ckRIFF, 0);
	mmioClose(hmmio, 0);

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
	ACE_OS::fprintf (stdout, "SORRY: This is not implemented\n");

	/**************************************************************
	 * Read a stream from a WAVE file and send it to the  network *
	 **************************************************************/
#if 0
	YARPSoundBuffer buffer;
	DeclareOutport(outport);

	outport.Register (_name, _netname);
	int frame_no = 0;
	ACE_OS::fprintf (stdout, "starting up simulation of a soundgrabber...\n");

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	/// load buffer from file.
	///
	const int buffer_size = 4096;
	buffer.Resize (buffer_size);

	/// actual load of data from file.

	///
	while (!IsTerminated())
	{
		YARPTime::DelayInSeconds (0.04);
	
		outport.Content().Refer (buffer);
		outport.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			start = cur;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
#endif
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
	int frame_no = 0;

	YARPSoundGrabber soundgrabber;
	YARPSoundBuffer buffer;

	//----------------------------------------------------------------------
	//  Port initialization
	//----------------------------------------------------------------------
	DeclareOutport(outport);
	outport.Register (_name, _netname);

	//----------------------------------------------------------------------
	//  Initialize the grabber
	//----------------------------------------------------------------------
	soundgrabber.initialize (_board_no,
							 _Channels,
							 _SamplesPerSec,
							 _BitsPerSample,
							 _BufferLength);
	soundgrabber.setVolume(_volume);

	/// alloc buffer.
	buffer.Resize (_BufferLength);

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
	double cur   = start;

	//----------------------------------------------------------------------
	// Main loop 
	//----------------------------------------------------------------------
	while (!IsTerminated())
	{
		soundgrabber.waitOnNewFrame ();
		unsigned char *tmp;
		soundgrabber.acquireBuffer(&tmp);

		memcpy (buffer.GetRawBuffer(), tmp, sizeof(unsigned char) * _BufferLength);
		outport.Content().Refer (buffer);
		outport.Write();

		soundgrabber.releaseBuffer (); // this could be release earlier?

		//----------------------------------------------------------------------
		//  Time measurement stuff
		//----------------------------------------------------------------------
		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f soundframes #%d acquired\r", (cur-start)/250, frame_no);
			start = cur;
		}
	}

	//----------------------------------------------------------------------
	//  destroy fg_net_data port
	//----------------------------------------------------------------------
	if (_fgnetdata)
	{
		if (m_fg_net_data != NULL)
			delete m_fg_net_data; 
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

	do {
		cout << "Type q+return to quit" << endl;
		cin >> c;
		ParseParams(argc, argv, 1);
	}
	while (c != 'q');

	_thread.End(-1);

	return YARP_OK;
}
