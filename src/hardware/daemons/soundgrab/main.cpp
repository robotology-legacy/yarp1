/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran Gonzalez#                      ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: main.cpp,v 1.14 2004-08-30 10:33:56 beltran Exp $
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>

#include <yarp/YARPImages.h>
#include <yarp/YARPSound.h>
#include <yarp/YARPSoundPortContent.h>
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPBottleContent.h>
#include <yarp/YARPConfigFile.h>

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>

#include <iostream>

using namespace std;

#if defined(__QNXEurobot__)

#	include <YARPEurobotSoundGrabber.h>
#	define DeclareOutport(x) YARPOutputPortOf<YARPSoundBuffer>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__WIN32Babybot__)

#	include <yarp/YARPBabybotSoundGrabber.h>
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

/**
 * Global parameters.
 */
char _name[512];
char _fgdataname[512];
char _netname[512];
bool _client          = false;
bool _simu            = false;
bool _fgnetdata       = false;
int  _board_no        = 0;
char __filename[256]  = "sound.ini";

/**
 * Default sound parameters.
 */
int _Channels      = 2;
int _SamplesPerSec = 44100;
int _BitsPerSample = 16;
int _BufferLength  = 8192;
int _volume        = 50;

extern int __debug_level;

/** 
  * Parses the parameters passed to the grabber.
  * 
  * @param argc The number of parameters
  * @param argv The array of pointers to the parameters characters arrays
  * @param visualize 
  * 
  * @return 
  */
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

#if !defined(__LinuxTest__)
/** 
  * Extends the YARPPort in order to implement the OnRead callback function.
  * It receives a bottle with information adjust the sound. To be used with fgadjuster photon 
  * application or with a similar application.
  */
class FgNetDataPort : public YARPInputPortOf<YARPBottle>
{
	protected:
		YARPBottle m_bottle;
		YARPSoundGrabber * m_gb;

	public:
		/** 
		  * The constructor.
		  * 
		  * @param gb The pointer to the soundgrabber pointer
		  */
		FgNetDataPort (YARPSoundGrabber * gb):YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS, YARP_UDP) 
		{ 
			m_gb = gb;
			m_bottle.reset();
		}

		/** 
		  * This is the callback function.
		  */
		virtual void OnRead(void);
};

/** 
  * The callback function.
  */
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

/** 
  * The main thread.
  */
class mainthread : public YARPThread
{
public:
	/** 
	  * The main body code.
	  */
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

/** 
  * This method acts as a client for another soundgrabber.
  * Simply reads the sound buffer form the network and puts it in a WAVE file.
  * 
  * @return YARP_OK
  */
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

/** 
  * This function simulates a recording and sends the simulated data into the network.
  * It is used to test this level plus clients and network connections.
  * 
  * @return 
  * 	-# YARP_OK everything was ok.
  * 	-# YARP_FAIL something didn't work as expected. 
  */
int 
mainthread::_runAsSimulation (void)
{
	HMMIO        hmmio;
	MMCKINFO     ckRIFF;
	MMCKINFO     ck;
	WAVEFORMATEX waveFormat;
	double start = YARPTime::GetTimeAsSeconds ();
	double cur   = start;
	
	YARPSoundBuffer buffer;

	ACE_OS::fprintf (stdout, "starting up simulation of a soundgrabber...\n");
	//----------------------------------------------------------------------
	//  Port initialization
	//----------------------------------------------------------------------
	DeclareOutport(outport);
	outport.Register (_name, _netname);
	
	/// alloc buffer.
	buffer.Resize (_BufferLength);

	int frame_no = 0;

	//----------------------------------------------------------------------
	//  Names for the input test files
	//----------------------------------------------------------------------
	char savename[512]; memset (savename, 0, 512); ACE_OS::sprintf (savename, "./input_test" );
	int file_number = 0;

	//----------------------------------------------------------------------
	//  Main loop
	//----------------------------------------------------------------------
	while (!IsTerminated())
	{
		//----------------------------------------------------------------------
		//  Compose the file name
		//----------------------------------------------------------------------
		memset (savename, 0, 512); ACE_OS::sprintf (savename, "./input_test%d.wav", file_number);
		file_number++;
		if (file_number == 4) file_number=0;

		//----------------------------------------------------------------------
		//  Open and initialize a WAVE file
		//  Note: I am using mmio functions. I have to figure out how to do
		//  this in QNX and Linux
		//----------------------------------------------------------------------
		hmmio = mmioOpen(savename,
						 NULL,
						 MMIO_READ | MMIO_ALLOCBUF);

		if ( hmmio == NULL)
		{
			ACE_OS::fprintf(stdout, "soundgrabber: %s file doesn't exits\n",savename);
			break;
		}

		//----------------------------------------------------------------------
		//  Read the WAVE header
		//----------------------------------------------------------------------
		ckRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		ckRIFF.cksize  = 0;
		if (mmioDescend(hmmio, (LPMMCKINFO)&ckRIFF, 0, MMIO_FINDRIFF)) 
		{
			ACE_OS::fprintf (stdout, "soundgrabber: This file doesn't contain a WAVE!\n");
			return YARP_FAIL;
		} 

		//----------------------------------------------------------------------
		//  Read the FMT stuff
		//----------------------------------------------------------------------
		ck.ckid   = mmioFOURCC('f', 'm', 't', ' ');
		ck.cksize = 0L;
		if (mmioDescend(hmmio, &ck, &ckRIFF, MMIO_FINDCHUNK)) 
		{
			ACE_OS::fprintf(stdout, "soundgrabber: This files doesn't contain a FMT chuck!\n");
			return YARP_FAIL;
		}

		//----------------------------------------------------------------------
		//  Read FMT chuck
		//----------------------------------------------------------------------
		if (mmioRead(hmmio, (HPSTR)&waveFormat, ck.cksize) != (LRESULT)ck.cksize)
		{
			ACE_OS::fprintf(stdout, "soundgrabber: Error reading the FMT chuck\n");
			return YARP_FAIL;
		}

		mmioAscend(hmmio, &ck, 0);

		//----------------------------------------------------------------------
		//  Read the Data chuck
		//----------------------------------------------------------------------
		ck.ckid   = mmioFOURCC('d', 'a', 't', 'a');
		ck.cksize = 0;

		if (mmioDescend(hmmio, &ck, &ckRIFF, MMIO_FINDCHUNK)) 
		{
			ACE_OS::fprintf(stdout,"soundgrabber: There is not Data chuck!\n");
			return YARP_FAIL;
		}

		int data_size   = 0;
		int readed_data = 0;
		int z = 0;
		
		for(z = 0; z < 250; z++) //Ten seconds of sound idependently of the size of the file
		{
			YARPTime::DelayInSeconds (0.04); //Delay 40 milliseconds

			readed_data = mmioRead(hmmio, (char *)buffer.GetRawBuffer(), _BufferLength);
			data_size += readed_data;

			if (!readed_data)
			{
				//ACE_OS::fprintf(stdout,"soundgrabber: I have finish reading the Wave file\n");
				//ACE_OS::fprintf(stdout,"soundgrabber: going back to the start of the file\n");
				mmioSeek(hmmio, data_size, SEEK_END); // Go back to the start position
				data_size = 0;
			}else
			{
				outport.Content().Refer (buffer);
				outport.Write();
			}

			frame_no++;
			if ((frame_no % 250) == 0)
			{
				cur = YARPTime::GetTimeAsSeconds ();
				ACE_OS::fprintf (stdout, "soundgrabber: average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
				start = cur;
			}
		}

		//----------------------------------------------------------------------
		//  Close the WAVE file
		//----------------------------------------------------------------------
		mmioAscend(hmmio, &ck, 0);
		mmioAscend(hmmio, &ckRIFF, 0);
		mmioClose(hmmio, 0);
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

/** 
  * This method instanciates an access to the low level driver.
  * It gets the sound data and sends it to the network. 
  *
  * @return 
  * 	-# YARP_OK everything was ok.
  * 	-# YARP_FAIL something went wrong
  */
int 
mainthread::_runAsNormally (void)
{
	int frame_no   = 0;
	int mute_value = 1;

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
	soundgrabber.setMute(mute_value);

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

	soundgrabber.setMute(0);
	soundgrabber.uninitialize ();

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

/** 
  * Obviously the main function.
  * 
  * @param argc The number of parameters.
  * @param argv The actual pointer to the parameters arrays.
  * 
  * @return YARP_OK Seems everything went perfect.
  */
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
