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
/// $Id: Headers.h,v 1.2 2004-10-08 22:11:06 gmetta Exp $
///
///

/**
 * \file Headers.h A few definitions for communication headers.
 * This is to be used only by private code within YARP OS library.
 */

#ifndef __Headersh__
#define __Headersh__


#include <yarp/YARPNetworkTypes.h>
#include <yarp/begin_pack_for_net.h>

/**
 * The YARP message header.
 */
struct MyMessageHeader
{
public:
	char key[2];
	NetInt32 len;
	char key2[2];

	/**
	 * Constructor.
	 */
	MyMessageHeader()
	{
		len = 0;
		SetBad();
	}

	/**
	 * Sets the header to represent a valid message.
	 */
	void SetGood()
    {
		key[0] = 'Y';
		key[1] = 'A';
		key2[0] = 'R';
		key2[1] = 'P';
    }
  
	/**
	 * Sets the header to represent an error message.
	 */
	void SetBad()
    {
		key[0] = 'y';
		key[1] = 'a';
		key2[0] = 'r';
		key2[1] = 'p';
    }

	/**
	 * Sets the length parameter of the message. The length
	 * represents the length of the next message block.
	 * @param n_len is the length in bytes.
	 */
	void SetLength(int n_len)
	{
		len = n_len;
	}
  
	/**
	 * Gets the length of the message block. 
	 * @return the length in bytes of the following chunk only if
	 * the message is a good one.
	 */
	int GetLength()
	{
		if (key[0] == 'Y' && key[1] == 'A' && key2[0] == 'R' && key2[1] == 'P')
		{
			return len;
		}
		else
		{
			return -1;
		}
	}
} PACKED_FOR_NET;


/**
 * No documentation yet for this class.
 */
class NewFragmentHeader
{
public:
	NetInt32 length;
	unsigned char checker;
	char tag;
	char more;
	char first;

	NewFragmentHeader() { checker='~';}
} PACKED_FOR_NET;

#include <yarp/end_pack_for_net.h>


#endif /// __Headersh__

