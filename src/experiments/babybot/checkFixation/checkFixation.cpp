/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
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
/// $Id: checkFixation.cpp,v 1.1 2004-06-07 18:32:17 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <YARPMath.h>

#include <YARPParseParameters.h>
#include <YARPVectorPortContent.h>

///
YARPInputPortOf<YVector> _inDisp (YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
YARPInputPortOf<YVector> _inTarget (YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
YARPOutputPortOf<YVector> _out (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

const char *DEFAULT_NAME = "/checkfixation";

double __vergenceTh = 2;
double __trackTh = (128/13 * 128/13)*2;
int __abortingTime = (int) (3/20.0)*1000;
double __correlationThreshold = 0.16;

bool _checkVergence(const YVector &in);
bool _checkTrack(const YVector &in);

int main(int argc, char *argv[])
{
	YARPString name;
	YARPString network_i;
	YARPString network_o;
	char buf[256];

	if (!YARPParseParameters::parse(argc, argv, "name", name))
		name = DEFAULT_NAME;
	
	if (!YARPParseParameters::parse(argc, argv, "neti", network_i))
		network_i = "default";
	
	if (!YARPParseParameters::parse(argc, argv, "neto", network_o))
		network_o = "default";
	
	/// images are coming from the input network.
	sprintf(buf, "%s/disparity/i", name.c_str());
	_inDisp.Register(buf, network_i.c_str());
	sprintf(buf, "%s/target/i", name.c_str());
	_inTarget.Register(buf, network_i.c_str());

	sprintf(buf, "%s/out", name.c_str());
	_out.Register(buf, network_o.c_str());

	YVector out(1);
	YVector target(2);
	YVector disp;

	target(1) = 128;
	target(2) = 128;

	out(1) = 0;

	bool dispF = false;
	bool trackF = false;
	bool abortF = false;
	int count = 0;

	while (1)
	{
		_inDisp.Read();
		//ACE_OS::printf("received\n");
		if (_inTarget.Read(0))
			target = _inTarget.Content();


		double vergence = _inDisp.Content()(1);
		
		if (_checkVergence(_inDisp.Content()))
		{
			out(1) = 0.5;
			dispF = true;
		}
		else
		{
			out(1) = 0.0;
			dispF = false;
		}

		if (_checkTrack(target))
		{
			out(1) += 0.5;
			trackF = true;
		}
		else
		{
			trackF = false;
		}

		/// check timeout
		if (_inDisp.Content()(2) < __correlationThreshold)
		{
			count++;
			if (count > __abortingTime)
			{
				out(1) = -1;
				abortF = true;
			}
		}
		else
		{
			count = 0;
			abortF = 0;
		}

		double tmp = target(1)*target(1) + target(2)*target(2);
		// print status
		if (abortF)
			ACE_OS::printf("%lf\t%lf\t-> ABORTED: %lf\t\n", vergence, tmp, out(1));
		else if (trackF&&dispF)
			ACE_OS::printf("%lf\t%lf\t-> FIXATED: %lf\t\r", vergence, tmp, out(1));
		else if (trackF)
			ACE_OS::printf("%lf\t%lf\t-> ALMOST_FIXATED: %lf\r", vergence, tmp, out(1));
		else
			ACE_OS::printf("%lf\t%lf\t-> TRACKING: %lf\t\t\r", vergence, tmp, out(1));

		_out.Content() = out;
		_out.Write();
	}
	return 0;
}

bool _checkVergence(const YVector &in)
{
	if ( fabs(in(1)) <= __vergenceTh )
		return true;

	return false;
}

bool _checkTrack(const YVector &in)
{
	double tmp;
	tmp = in(1)*in(1);
	tmp += in(2)*in(2);

	if (tmp < __trackTh)
		return true;

	return false;
}