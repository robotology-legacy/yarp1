/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: grabber.cpp,v 1.5 2004-12-13 11:32:58 gmetta Exp $
///
///



#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>

#include <yarp/YARPImages.h>
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPBottleContent.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPRobotHardware.h>

#include <iostream>

///
/// global params.
int		_sizex		= -1;
int		_sizey		= -1;
int		_yoffset	= 0;
char	_name[512];
char	_fgdataname[512];
char	_netname[512];
bool	_simu		= false;
bool	_logp		= false;
int		_board_no	= 0;
bool	_fgnetdata	= false;
bool	_help		= false;

extern int __debug_level;

int PrintHelp (void)
{
	ACE_OS::fprintf (stdout, "USAGE:\n");
	ACE_OS::fprintf (stdout, "--help, print help and exit\n");
	ACE_OS::fprintf (stdout, "--name <str>, use <str> as port name prefix (don't forget the leading /)\n");
	ACE_OS::fprintf (stdout, "--w <int>, set the acquisition width\n");
	ACE_OS::fprintf (stdout, "--h <int>, set the acquisition height\n");
	ACE_OS::fprintf (stdout, "--b <int>, board number (0 or 1)\n");
	ACE_OS::fprintf (stdout, "--s, simulation mode\n");
	ACE_OS::fprintf (stdout, "--l, output log polar images\n");
	ACE_OS::fprintf (stdout, "--net <str>, define the network name (in a multi-network configuration)\n");
	ACE_OS::fprintf (stdout, "--o <int>, set acquisition vertical offset\n");
	ACE_OS::fprintf (stdout, "--f, activate external control of acquisition parameters (through YARPBottle messages)\n");

	return YARP_OK;
}


int ParseParams (int argc, char *argv[]) 
{
	ACE_OS::sprintf (_name, "/%s/o:img", argv[0]);
	ACE_OS::sprintf (_fgdataname,"/%s/i:fgdata", argv[0]);
	ACE_OS::sprintf (_netname, "default");

	YARPString tmps;

	if (YARPParseParameters::parse(argc, argv, "help") ||
		argc == 1 ||
		YARPParseParameters::parse(argc, argv, "?") ||
		YARPParseParameters::parse(argc, argv, "-help"))
	{
		_help = true;
		return YARP_OK;
	}

	if (YARPParseParameters::parse(argc, argv, "-name", tmps))
	{
		ACE_OS::sprintf (_name, "%s/o:img", tmps.c_str());
		ACE_OS::sprintf (_fgdataname,"%s/i:fgdata", tmps.c_str());
	}

	if (YARPParseParameters::parse(argc, argv, "-w", &_sizex))
	{
		ACE_ASSERT (_sizex <= 384 && _sizex > 0);
		ACE_ASSERT ((_sizex % 8) == 0);
	}

	if (YARPParseParameters::parse(argc, argv, "-h", &_sizey))
	{
		ACE_ASSERT (_sizey <= 272 && _sizey > 0);
	}

	if (YARPParseParameters::parse(argc, argv, "-b", &_board_no))
	{
		ACE_ASSERT (_board_no >= 0 && _board_no <= 1);
	}

	if (YARPParseParameters::parse(argc, argv, "-s"))
	{
		ACE_OS::fprintf (stdout, "simulating a grabber...\n");
		_simu = true;
	}

	if (YARPParseParameters::parse(argc, argv, "-l"))
	{
		ACE_OS::fprintf (stdout, "logpolar mode\n");
		_logp = true;
	}
	
	if (YARPParseParameters::parse(argc, argv, "-net", tmps))
	{
		ACE_OS::fprintf (stdout, "sending to network : %s\n", tmps.c_str());
		ACE_OS::sprintf (_netname, "%s", tmps.c_str());
	}

	if (YARPParseParameters::parse(argc, argv, "-f"))
	{
		ACE_OS::fprintf(stdout, "grabber receiving data from network mode...\n");
		_fgnetdata = true;
	}
			
	YARPParseParameters::parse(argc, argv, "-o", &_yoffset);

	if (_sizex == -1 && _sizey != -1)
		_sizex = _sizey;
	else
	if (_sizex != -1 && _sizey == -1)
		_sizey = _sizex;
	else
	if (_sizex == -1 && _sizey == -1)
		_sizex = _sizey = 128;

	using namespace _logpolarParams;
	if (_logp)
	{
		_sizex = _xsize;
		_sizey = _ysize;
	}

	return YARP_OK; 
}


// =====================================================================================
//        Class:  FgNetDataPort
// 
//  Description:  This class extends a YARPPort in order to implement the OnRead callback function
// 				  It receives a bottle with information to adjust the images in the framegrabber
// 				  To be used to fgadjuster photon application. 
// 
//       Author:  Ing. Carlos Beltran
//      Created:  15/01/2003 10:36:00 W. Europe Standard Time
//     Revision:  21/01/2004: Removed some prints
// =====================================================================================

class FgNetDataPort : public YARPInputPortOf<YARPBottle>
{
	protected:
		YARPBottle m_bottle;
		YARPGrabber * m_gb;
		unsigned int m_bright;
		unsigned int m_hue;
		unsigned int m_contrast;
		unsigned int m_satu;
		unsigned int m_satv;
		int m_lnotch;
		int m_ldec;
		int m_peak;
		int m_cagc;
		int m_ckill;
		int m_range;
		int m_ysleep;
		int m_csleep;
		int m_crush;
		int m_gamma;
		int m_dithf;

	public:
		FgNetDataPort (YARPGrabber * gb):YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS, YARP_UDP) 
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
	///printf("Accesing OnRead\n");
	Read ();
	m_bottle = Content();
	m_bottle.rewind();
	m_bottle.readInt((int *)&m_bright);
	m_bottle.readInt((int *)&m_hue);
	m_bottle.readInt((int *)&m_contrast);
	m_bottle.readInt((int *)&m_satu);
	m_bottle.readInt((int *)&m_satv);
	m_bottle.readInt((int *)&m_lnotch);
	m_bottle.readInt((int *)&m_ldec);
	m_bottle.readInt((int *)&m_peak);
	m_bottle.readInt((int *)&m_cagc);
	m_bottle.readInt((int *)&m_ckill);
	m_bottle.readInt((int *)&m_range);
	m_bottle.readInt((int *)&m_ysleep);
	m_bottle.readInt((int *)&m_csleep);
	m_bottle.readInt((int *)&m_crush);
	m_bottle.readInt((int *)&m_gamma);
	m_bottle.readInt((int *)&m_dithf);

	///m_bottle.display();

	m_gb->setBright((unsigned int)m_bright);
	m_gb->setHue((unsigned int)m_hue);
	m_gb->setContrast((unsigned int)m_contrast);
	m_gb->setSatU((unsigned int)m_satu);
	m_gb->setSatV((unsigned int)m_satv);
	m_gb->setLNotch(m_lnotch);
	m_gb->setLDec(m_ldec);
	m_gb->setPeak(m_peak);
	m_gb->setCagc(m_cagc);
	m_gb->setCkill(m_ckill);
	m_gb->setRange(m_range);
	m_gb->setYsleep(m_ysleep);
	m_gb->setCsleep(m_csleep);
	m_gb->setCrush(m_crush);
	m_gb->setGamma(m_gamma);
	m_gb->setDithFrame(m_dithf);
}


///
///
///
///
class mainthread : public YARPThread
{
public:
	virtual void Body (void)
	{
		if (_simu && !_logp)
		{
			_runAsSimulation ();
			return;
		}
		else
		if (_simu && _logp)
		{
			_runAsLogpolarSimulation ();
			return;
		}
		else
		if (_logp)
		{
			_runAsLogpolar ();
			return;
		}
		else
		{
			_runAsCartesian ();
			return;
		}
	}

	int _runAsSimulation (void);
	int _runAsLogpolarSimulation (void);
	int _runAsLogpolar (void);
	int _runAsCartesian (void);
};


int mainthread::_runAsSimulation (void)
{
	YARPImageOf<YarpPixelBGR> img;
	img.Resize (_sizex, _sizey);
	img.Zero ();

	YARPOutputPortOf<YARPGenericImage> outport(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);
	outport.Register (_name, _netname);

	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up simulation of a grabber...\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", _sizex, _sizey);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!IsTerminated())
	{
		YARPTime::DelayInSeconds (0.04);
	
		img.Zero ();
		*(img.GetRawBuffer() + (frame_no % img.GetAllocatedDataSize())) = -1;

		outport.Content().Refer (img);
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
	return YARP_OK;
}

int mainthread::_runAsLogpolarSimulation (void)
{
	using namespace _logpolarParams;

	YARPImageOf<YarpPixelBGR> img;
	YARPImageOf<YarpPixelMono> lp;
	
	YARPLogpolarSampler sampler;

	ACE_ASSERT (_xsize == _ysize);
	ACE_ASSERT (_xsize == _sizex && _ysize == _sizey);

	img.Resize (_xsize, _ysize);
	lp.Resize (_stheta, _srho);

	YARPOutputPortOf<YARPGenericImage> out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);
	out.Register (_name, _netname);
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up simulation of a grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is logpolar\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", _xsize, _ysize);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	char * path = ACE_OS::getenv ("YARP_ROOT");
	char filename[512];
	ACE_OS::sprintf (filename, "%s/conf/test_grabber.ppm", path);
	YARPImageFile::Read (filename, img);

	while (!IsTerminated())
	{
		YARPTime::DelayInSeconds (0.04);
		
		/// blink
		if ((frame_no % 2) == 0)
			*(img.GetRawBuffer() + 128 * 256 * 3 + 128 * 3) = -1;
		else
			*(img.GetRawBuffer() + 128 * 256 * 3 + 128 * 3) = 0;

		sampler.Cartesian2Logpolar (img, lp);

		/// sends the buffer.
		out.Content().Refer (lp);
		out.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			start = cur;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

int mainthread::_runAsLogpolar (void)
{
	YARPGrabberParams params;
	using namespace _logpolarParams;

	YARPGrabber grabber;

	YARPImageOf<YarpPixelBGR> img;
	YARPImageOf<YarpPixelMono> lp;
	
	YARPLogpolarSampler sampler;

	ACE_ASSERT (_xsize == _ysize);
	ACE_ASSERT (_xsize == _sizex && _ysize == _sizey);

	img.Resize (_xsize, _ysize);
	lp.Resize (_stheta, _srho);

	YARPOutputPortOf<YARPGenericImage> out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);
	out.Register (_name, _netname);

	/// params to be passed from the command line.
	params._unit_number = _board_no;
	params._video_type = 0;
	params._size_x = _xsize;
	params._size_y = _ysize;
	params._offset_y = _yoffset;
	ACE_OS::fprintf(stdout, "Setting yoffset:%d\n", _yoffset);
	grabber.initialize (params);

	//Activate port to receive image adjustment data
	FgNetDataPort  * m_fg_net_data;
	if (_fgnetdata)
	{
		m_fg_net_data = new FgNetDataPort(&grabber);
		m_fg_net_data->Register (_fgdataname,_netname);
	}

	int w = -1, h = -1;
	grabber.getWidth (&w);
	grabber.getHeight (&h);

	unsigned char *buffer = NULL;
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is logpolar\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", w, h);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!IsTerminated())
	{
		grabber.waitOnNewFrame ();
		grabber.acquireBuffer (&buffer);
	
		/// fills the actual image buffer.
		memcpy((unsigned char *)img.GetRawBuffer(), buffer, _xsize * _xsize * 3);
		
		grabber.releaseBuffer ();

		sampler.Cartesian2Logpolar (img, lp);

		/// sends the buffer.
		out.Content().Refer (lp);
		out.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			start = cur;
		}
	}

	if (_fgnetdata)
	{
		if (m_fg_net_data != NULL)
			delete m_fg_net_data; //can this be done better? (closind the port?)
	}

	grabber.uninitialize ();
	
	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

int mainthread::_runAsCartesian (void)
{
	YARPGrabber grabber;
	YARPImageOf<YarpPixelBGR> img;
	img.Resize (_sizex, _sizey);

	YARPOutputPortOf<YARPGenericImage> outport(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);
	outport.Register (_name, _netname);

	/// params to be passed from the command line.
	grabber.initialize (_board_no, _sizex, _sizey);
	
	FgNetDataPort  * m_fg_net_data;
	if (_fgnetdata)
	{
		m_fg_net_data = new FgNetDataPort(&grabber);
		m_fg_net_data->Register (_fgdataname,_netname);
	}

	int w = -1, h = -1;
	grabber.getWidth (&w);
	grabber.getHeight (&h);

	unsigned char *buffer = NULL;
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is cartesian\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", w, h);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!IsTerminated())
	{
		grabber.waitOnNewFrame ();
		grabber.acquireBuffer(&buffer);

		memcpy((unsigned char *)img.GetRawBuffer(), buffer, _sizex * _sizey * 3);

		grabber.releaseBuffer ();

		/// sends the buffer.
		outport.Content().Refer (img);
		outport.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			start = cur;
		}
	}

	//destroy fg_net_data port
	if (_fgnetdata)
	{
		if (m_fg_net_data != NULL)
			delete m_fg_net_data; //can this be done better? (closind the port?)
	}

	grabber.uninitialize ();

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

///
///
///
int main (int argc, char *argv[])
{
	using namespace std;
	YARPScheduler::setHighResScheduling ();

	ParseParams (argc, argv);
	if (_help)
	{
		PrintHelp ();
		return YARP_OK;
	}

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



