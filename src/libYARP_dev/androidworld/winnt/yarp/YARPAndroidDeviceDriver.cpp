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
/// $Id: YARPAndroidDeviceDriver.cpp,v 1.1 2004-07-13 13:21:07 babybot Exp $
///
///

#include "YARPAndroidDeviceDriver.h"

YARPAndroidDeviceDriver::YARPAndroidDeviceDriver():
YARPDeviceDriver<YARPNullSemaphore, YARPAndroidDeviceDriver>(ADCCmds)
{
	_pport = (unsigned short) 0x378;
	_chipSel = 0;
	_boardSel = 0;

	m_cmds[CMDAIReadScan] = &YARPAndroidDeviceDriver::ai_read_scan;
	m_cmds[CMDAIReadChannel] = &YARPAndroidDeviceDriver::ai_read_channel;
	m_cmds[CMDScanSetup] = &YARPAndroidDeviceDriver::scan_setup;
}

int YARPAndroidDeviceDriver::open (void *d)
{
	AndroidOpenParameters *tmp = (AndroidOpenParameters *) d;

	_pport = tmp->pport;

	// init taken "as it was" from android world example
	selChip(0,0,0);
	outPort2(0,0);
	readMSB();

	selChip(0,0,1);	// disable all chip
	outPort2(0,0);
	readMSB();
	
	selChip(0,0,0);
	outPort2(0,0);
	readMSB();
	//////////////////////////

	return YARP_OK;
}

int YARPAndroidDeviceDriver::close()
{
	return YARP_OK;
}

// read one of the 8 channels from a specified chipe and board
unsigned char YARPAndroidDeviceDriver::readADC(unsigned char chip, unsigned char board, int channel)
{
	int ch[__AndroidNChannels][__AndroidNChips] =
	{
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{1, 0, 0, 1},
		{1, 1, 0, 1},
		{1, 0, 1, 0},
		{1, 1, 1, 0},
		{1, 0, 1, 1},
		{1, 1, 1, 1}
	};

	unsigned char result = 0;

	_chipSel = chip;
	_boardSel = board;
	_enable = 0;

	_parOut(0,1);
	_parOut(1,1);
	_parOut(0,1);
	
	_parOut(0,ch[channel][0]);
	_parOut(1,ch[channel][0]);
	_parOut(0,ch[channel][0]);
	
	_parOut(0,ch[channel][1]);
	_parOut(1,ch[channel][1]);
	_parOut(1,ch[channel][1]);
	_parOut(0,ch[channel][1]);

	_parOut(0,ch[channel][2]);
	_parOut(1,ch[channel][2]);
	_parOut(1,ch[channel][2]);
	_parOut(0,ch[channel][2]);

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	_parOut(0,ch[channel][3]);

	// receive digital signal from the adc
	_parOut(1,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	
	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	unsigned char out = _parOut(1,ch[channel][3]);
	result += (out << 7)&128;	// MSB

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	out = _parOut(1,ch[channel][3]);
	result += (out << 6)&64;

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	out = _parOut(1,ch[channel][3]);
	result += (out << 5)&32;

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	out = _parOut(1,ch[channel][3]);
	result += (out << 4)&16;

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	out = _parOut(1,ch[channel][3]);
	result += (out << 3)&8;

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	out = _parOut(1,ch[channel][3]);
	result += (out << 2)&4;

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	out = _parOut(1,ch[channel][3]);
	result += (out << 1)&2;

	_parOut(0,ch[channel][3]);
	_parOut(1,ch[channel][3]);
	out = _parOut(1,ch[channel][3]);
	result += (out)&1;

	_parOut(0,ch[channel][3]);

	return result;
}


unsigned char YARPAndroidDeviceDriver::read(unsigned char chip, unsigned char board, int channel)
{
	selChip(chip, board, 0);
	outPort2(0,0);

	unsigned char ret = readADC(chip, board, channel);

	selChip(chip, board, 1);
	outPort2(0,0);

	return ret;
}

int YARPAndroidDeviceDriver::ai_read_channel(void *d)
{
	AndroidSingleChannel *tmp = (AndroidSingleChannel *)d;

	*(tmp->data) = read(tmp->chip, _boardSel, tmp->channel);

	return YARP_OK;
}
	
int YARPAndroidDeviceDriver:: ai_read_scan(void *d)
{
	int i = 0;
	int j = 0;
	int z = 0;

	for (j = 0; j<__AndroidNChips; j++)
		for(i = 0; i<__AndroidNChannels; i++)
		{
			_temp_data[z] = read(j, _boardSel, i);
			z++;
		}

	memcpy(d, _temp_data, __AndroidNInput);
	return YARP_OK;
}

int YARPAndroidDeviceDriver:: scan_setup(void *b)
{
	AndroidScanSetupParameters *tmp = (AndroidScanSetupParameters *) b;

	_boardSel = tmp->nBoard;

	return YARP_OK;
}