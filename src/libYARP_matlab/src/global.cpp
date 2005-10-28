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
/// $Id: global.cpp,v 1.3 2005-10-28 16:11:08 natta Exp $
///
///

///
/// I store here my own global stuff, in particular a list of ports and their attributes.
/// - I expect to recast ports to their original types for using them
/// - This memory is not deallocated unless matlab issues a clean unloading the DLL.
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPMath.h>
//#include <yarp/YARPVectorPortContent.h>
#include <yarp/YARPBottle.h>
//#include <yarp/YARPBottleContent.h>
#include <yarp/YARPImage.h>
//#include <yarp/YARPImagePortContent.h>

#include "../include/yarp_private/global.h"

///
///
/// local internal prototypes.
int stringToProtocol (const char *s);
int stringToBuffering (const char *s, bool direction);
char *typeToString(enum _dataType type, char *buffer);

/// operation callbacks.
int create_port (void *params);
int register_port (void *params);
int unregister_port (void *params);
int destroy_port (void *params);
int list_port (void *params);
int read_port (void *params);
int write_port (void *params);
int porter (void *params);

///
///
/// hopefully gets called before unloading the DLL.
class _listHandler 
{
protected:
	_portEntry *_list;

public:
	_listHandler()
	{
		_list = new _portEntry[MAX_PORTS];
		ACE_ASSERT (_list != NULL);

		for (int i = 0; i < MAX_PORTS; i++)
		{
			memset (_list+i, 0, sizeof(_portEntry));
			_list[i]._type = MX_YARP_INVALID;
		}
	}

	~_listHandler()
	{
	}

	///
	/// to be called before cleaning up the DLL.
	void explicitDestroy (void)
	{
		if (_list != NULL)
		{
			for (int i = 0; i < MAX_PORTS; i++)
			{
				_dispatchParams par;
				memset (&par, 0, sizeof(_dispatchParams));
				par._portnumber = i;
				destroy_port ((void *)&par);
			}

			delete[] _list;
			_list = NULL;
		}
	}

	/// not checked.
	_portEntry& operator[] (int i) { return _list[i]; }
};

/// the list variable. 
static _listHandler __list;

/// C linkage.
void cleanUpList(void)
{
	__list.explicitDestroy();
}

///
///
/// a list of pointers to functions.
typedef int (*PV) (void *);

struct _actionEntryTag
{
	PV _function;
	char *_operation;
};
typedef struct _actionEntryTag _actionEntry;

const int NUM_OF_ACTIONS = 8;
static _actionEntry _actions[NUM_OF_ACTIONS] =
{
	{ (PV)create_port, "create" },
	{ (PV)register_port, "register" },
	{ (PV)unregister_port, "unregister" },
	{ (PV)destroy_port, "destroy" },
	{ (PV)list_port, "list" },
	{ (PV)read_port, "read" },
	{ (PV)write_port, "write" },
	{ (PV)porter, "porter" }
};

///
int stringToProtocol (const char *s)
{
	if (strncmp (s, "tcp", 3) == 0)
		return YARP_TCP;
	else
	if (strncmp (s, "udp", 3) == 0)
		return YARP_UDP;
	else
	if (strncmp (s, "mcast", 4) == 0)
		return YARP_MCAST;
	else
		/// default.
		return YARP_TCP;
}

int stringToBuffering (const char *s, bool direction)
{
	/*
	also from yarp, the buffering

	input port:
		NO_BUFFERS,
		DOUBLE_BUFFERS,
		TRIPLE_BUFFERS,
		DEFAULT_BUFFERS = TRIPLE_BUFFERS

	output port:
		MANY_OUTPUTS,
		SINGLE_OUTPUT,
		DEFAULT_OUTPUTS = MANY_OUTPUTS
*/
	if (strncmp (s, "no_buffers", 10) == 0)
		return YARPInputPort::NO_BUFFERS;
	else
	if (strncmp (s, "double_buffers", 14) == 0)
		return YARPInputPort::DOUBLE_BUFFERS;
	else
	if (strncmp (s, "triple_buffers", 14) == 0)
		return YARPInputPort::TRIPLE_BUFFERS;
	else
	if (strncmp (s, "many_outputs", 12) == 0)
		return YARPOutputPort::MANY_OUTPUTS;
	else
	if (strncmp (s, "single_output", 13) == 0)
		return YARPOutputPort::SINGLE_OUTPUT;
	else
	{
		if (direction) 
			return YARPInputPort::DEFAULT_BUFFERS;
		else
			return YARPOutputPort::DEFAULT_OUTPUTS;
	}
}

/// from the yarp protocol enumeration.
///	YARP_UDP					= 0,
///	YARP_TCP					= 1,
///	YARP_MCAST					= 2,
///	YARP_QNET					= 3,
/// YARP_SHMEM					= 4,

int dispatcher (const char *operation, void *params)
{
	for (int i = 0; i < NUM_OF_ACTIONS; i++)
	{
		if (strcmp (operation, _actions[i]._operation) == 0)
			return (*_actions[i]._function)(params);
	}

	return -1;
}

///
///
/// messing up things here :)
///
#define PORT_CREATE(x, y) \
		case MX_YARP_##x: \
		{ \
				if (par._direction) \
					myport._port = (void *) new YARPInputPortOf<y> (stringToBuffering(par._extra_content, par._direction), stringToProtocol(par._protocol)); \
				else \
					myport._port = (void *) new YARPOutputPortOf<y> (stringToBuffering(par._extra_content, par._direction), stringToProtocol(par._protocol)); \
				myport._type = MX_YARP_##x; \
				myport._direction = par._direction; \
				myport._name = NULL; \
		} \
		break;

///
///
#define PORT_REGISTER(x, y) \
  	case MX_YARP_##x: \
		{ \
			if (myport._direction) \
			{ \
				YARPInputPortOf<y> *port = (YARPInputPortOf<y> *)myport._port; \
				return_value = port->Register (par._portname, par._network); \
				if (return_value >= 0) \
				{ \
					int len = strlen(par._portname); \
					myport._name = new char[len+1]; \
					strcpy (myport._name, par._portname); \
					myport._name[len] = 0; \
				} \
			} \
			else \
			{ \
				YARPOutputPortOf<int> *port = (YARPOutputPortOf<int> *)myport._port; \
				return_value = port->Register (par._portname, par._network); \
				if (return_value >= 0) \
				{ \
					int len = strlen(par._portname); \
					myport._name = new char[len+1]; \
					strcpy (myport._name, par._portname); \
					myport._name[len] = 0; \
				} \
			} \
		} \
	break;

///
///
#define PORT_UNREGISTER(x, y) \
	case MX_YARP_##x: \
		{ \
			if (myport._direction) \
			{ \
				YARPInputPortOf<y> *port = (YARPInputPortOf<y> *)myport._port; \
				return_value = port->Unregister (); \
				if (myport._name != NULL) delete[] myport._name; \
				myport._name = NULL; \
			} \
			else \
			{ \
				YARPOutputPortOf<y> *port = (YARPOutputPortOf<y> *)myport._port; \
				return_value = port->Unregister (); \
				if (myport._name != NULL) delete[] myport._name; \
				myport._name = NULL; \
			} \
		} \
		break; \

///
///
#define PORT_DESTROY(x, y) \
	case MX_YARP_##x: \
		{ \
			if (myport._direction) \
			{ \
				YARPInputPortOf<y> *port = (YARPInputPortOf<y> *)myport._port; \
				delete port; \
				myport._port = NULL; \
				myport._type = MX_YARP_INVALID; \
				myport._direction = false; \
				myport._name = NULL; \
			} \
			else \
			{ \
				YARPOutputPortOf<y> *port = (YARPOutputPortOf<y> *)myport._port; \
				delete port; \
				myport._port = NULL; \
				myport._type = MX_YARP_INVALID; \
				myport._direction = false; \
				myport._name = NULL; \
			} \
		} \
		break;

///
///
///

///
///
/// create a port of the given type and stores into list.
int create_port (void *params)
{
	_dispatchParams& par = *(_dispatchParams *)params;
	int i, portno = -1;

	for (i = 0; i < MAX_PORTS; i++)
	{
		if (__list[i]._port == NULL)
		{
			portno = i;
			break;
		}
	}

	if (portno == -1)
		return -1;

	///
	_portEntry& myport = __list[portno];

	switch (par._type)
	{
		PORT_CREATE (INT, int);
		PORT_CREATE (DOUBLE, double);
		PORT_CREATE (FLOAT, float);
		PORT_CREATE (YVECTOR, YVector);
		PORT_CREATE (BOTTLE, YARPBottle);
		PORT_CREATE (IMAGE, YARPGenericImage);

		default:
			return -1;
			break;
	}

	return portno;
}

///
///
///
int register_port (void *params)
{
	_dispatchParams& par = *(_dispatchParams *)params;
	int portno = -1;
	int return_value = -1;

	portno = par._portnumber;
	if (portno < 0 || portno >= MAX_PORTS)
		return -1;

	_portEntry& myport = __list[portno];
	if (par._portname == NULL ||
		par._network == NULL ||
		myport._name != NULL)	/// already registered.
		return -1;

	switch (myport._type)
	{
		PORT_REGISTER (INT, int);
		PORT_REGISTER (DOUBLE, double);
		PORT_REGISTER (FLOAT, float);
		PORT_REGISTER (YVECTOR, YVector);
		PORT_REGISTER (BOTTLE, YARPBottle);
		PORT_REGISTER (IMAGE, YARPGenericImage);

		default:
			return -1;
			break;
	}

	return return_value;
}

///
///
///
int unregister_port (void *params)
{
	_dispatchParams& par = *(_dispatchParams *)params;
	int portno = -1;
	int return_value = -1;

	portno = par._portnumber;
	if (portno < 0 || portno >= MAX_PORTS)
		return -1;

	_portEntry& myport = __list[portno];
	if (myport._name == NULL)
		return -1;

	switch (myport._type)
	{
		PORT_UNREGISTER(INT, int);
		PORT_UNREGISTER(DOUBLE, double);
		PORT_UNREGISTER(FLOAT, float);
		PORT_UNREGISTER(YVECTOR, YVector);
		PORT_UNREGISTER(BOTTLE, YARPBottle);
		PORT_UNREGISTER(IMAGE, YARPGenericImage);

		default:
			return -1;
			break;
	}

	return return_value;
}

///
///
///
int destroy_port (void *params)
{
	_dispatchParams& par = *(_dispatchParams *)params;
	int portno = -1;
	int return_value = 0;

	portno = par._portnumber;
	if (portno < 0 || portno >= MAX_PORTS)
		return -1;

	_portEntry& myport = __list[portno];

	if (myport._name != NULL)
	{
		int r = unregister_port (params);
		if (r < 0)
			return r;
	}

	switch (myport._type)
	{
		PORT_DESTROY (INT, int);
		PORT_DESTROY (DOUBLE, double);
		PORT_DESTROY (FLOAT, float);
		PORT_DESTROY (YVECTOR, YVector);
		PORT_DESTROY (BOTTLE, YARPBottle);
		PORT_DESTROY (IMAGE, YARPGenericImage);

		default:
			return -1;
			break;
	}

	return return_value;
}

///
///
///
char *typeToString(enum _dataType type, char *buffer)
{
	switch (type)
	{
	case MX_YARP_INT:
		strncpy (buffer, "int", 3);
		buffer[3] = 0;
		break;

	case MX_YARP_DOUBLE:
		strncpy (buffer, "double", 6);
		buffer[6] = 0;
		break;

	case MX_YARP_YVECTOR:
		strncpy (buffer, "vector", 6);
		buffer[6] = 0;
		break;

	case MX_YARP_IMAGE:
		strncpy (buffer, "image", 5);
		buffer[5] = 0;
		break;

	case MX_YARP_FLOAT:
		strncpy (buffer, "float", 5);
		buffer[5] = 0;
		break;

	case MX_YARP_BOTTLE:
		strncpy (buffer, "bottle", 6);
		buffer[6] = 0;
		break;

	default:
		strncpy (buffer, "invalid type", 12);
		buffer[12] = 0;
		break;
	}

	return buffer;
}

///
///
/// list active ports.
int list_port (void *params)
{
	int i;
	_dispatchParams& par = *(_dispatchParams *)params;

	if (par._extra_content == NULL)
		return -1;

	char *tmp = par._extra_content;
	sprintf (tmp, "list of active ports\n");
	tmp = par._extra_content + strlen (par._extra_content);

	char buffer[25];

	for (i = 0; i < MAX_PORTS; i++)
	{
		_portEntry& myport = __list[i];
		if (myport._port != NULL)
		{
			sprintf (tmp, "$$ %d of type %s of %s, registered as %s\n", i, typeToString(myport._type, buffer), (myport._direction)?"input":"output", (myport._name != NULL)?myport._name:"(Null)");
		}
		else
		{
			sprintf (tmp, "$$ slot %d empty\n", i);
		}

		tmp = par._extra_content + strlen (par._extra_content);
	}
	return 0;
}

#define PORT_READ(x, y) \
		case MX_YARP_##x: \
		{ \
			YARPInputPortOf<y> *port = (YARPInputPortOf<y> *)myport._port; \
			if (port->Read((par._extra_params)?true:false)) \
			{ \
				par._extra_content = (char *)&(port->Content()); \
				return_value = 0; \
				par._type = myport._type; \
			} \
			else \
			{ \
				par._extra_content = NULL; \
				return_value = -2; \
				par._type = MX_YARP_INVALID; \
			} \
			par._sizex = 1; \
			par._sizey = 1; \
		} \
		break;


#define PORT_WRITE(x, y) \
		case MX_YARP_##x: \
		{ \
			YARPOutputPortOf<y> *port = (YARPOutputPortOf<y> *)myport._port; \
			if (par._sizex != 1 || par._sizey != 1) \
				return_value = -1; \
			else \
			{ \
				port->Content() = y((*(double *)par._extra_content) + .5); \
				port->Write((par._extra_params)?true:false); \
				return_value = 0; \
			} \
		} \
		break;

///
///
///
int read_port (void *params)
{
	_dispatchParams& par = *(_dispatchParams *)params;
	int portno = -1;
	int return_value = -1;

	portno = par._portnumber;
	if (portno < 0 || portno >= MAX_PORTS)
		return -1;

	_portEntry& myport = __list[portno];
	if (myport._port == NULL ||
		myport._type == MX_YARP_INVALID ||
		myport._direction == false ||
		myport._name == NULL)
		return -1;

	switch (myport._type)
	{
		PORT_READ (INT, int);
		PORT_READ (DOUBLE, double);
		PORT_READ (FLOAT, float);

		case MX_YARP_YVECTOR:
		{
			YARPInputPortOf<YVector> *port = (YARPInputPortOf<YVector> *)myport._port;
			if (port->Read((par._extra_params)?true:false))
			{
				YVector& v = port->Content();
				par._extra_content = (char *)v.data();
				return_value = 0;
				par._type = myport._type;
				par._sizex = v.Length();
			}
			else
			{
				par._extra_content = NULL;
				return_value = -2;
				par._type = MX_YARP_INVALID;
				par._sizex = 0;
			}
			par._sizey = 1;
		}
		break;

		case MX_YARP_BOTTLE:
		{
			YARPInputPortOf<YARPBottle> *port = (YARPInputPortOf<YARPBottle> *)myport._port;
			if (port->Read((par._extra_params)?true:false))
			{
				YARPBottle& b = port->Content();
				par._extra_content = (char *)b.getDataPtr();
				par._portname = (char *)b.getID().c_str();
				par._sizex = b.getTopInBytes();
				return_value = 0;
				par._type = myport._type;
			}
			else
			{
				par._extra_content = NULL;
				par._portname = NULL;
				return_value = -2;
				par._type = MX_YARP_INVALID;
				par._sizex = 0;
			}
			par._sizey = 1;
		}
		break;

		case MX_YARP_IMAGE:
		{
			YARPInputPortOf<YARPGenericImage> *port = (YARPInputPortOf<YARPGenericImage> *)myport._port;
			if (port->Read((par._extra_params)?true:false))
			{
				YARPGenericImage& b = port->Content();
				par._sizey = b.GetHeight();
				par._sizex = b.GetWidth();
				par._extra_content = (char *)b.GetRawBuffer();
				
				par._extra_params = ((b.GetID() > 0)?(b.GetID()):(-b.GetID())) & 0x0000ffff;
				par._extra_params |= ((b.GetPadding() << 16) & 0xffff0000);
				return_value = 0;
				par._type = myport._type;
			}
			else
			{
				/// nothing to read.
				par._extra_content = NULL;
				par._extra_params = 0;
				return_value = -2;
				par._type = MX_YARP_INVALID;
				par._sizex = 0;
				par._sizey = 0;
			}
		}
		break;

		default:
			return -1;
			break;
	}

	return return_value;
}


///
///
template <class T>
int matlab_to_yarp (T* in, T* out, int w, int h, int padding)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			*out++ = *(in + h * j + i);
		}
		out = (T*)(((char *)out)+padding);
	}

	return 0;
}

template <class T>
int matlab_to_yarp3 (T* in, T* out, int w, int h, int padding)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			*out++ = *(in + h * j + i);
			*out++ = *(in + h * w + h * j + i);
			*out++ = *(in + 2 * h * w + h * j + i);
		}
		out = (T*)(((char *)out)+padding);
	}

	return 0;
}

///
///
///
int write_port (void *params)
{
	_dispatchParams& par = *(_dispatchParams *)params;
	int portno = -1;
	int return_value = -1;

	portno = par._portnumber;
	if (portno < 0 || portno >= MAX_PORTS)
		return -1;

	_portEntry& myport = __list[portno];
	if (myport._port == NULL ||
		myport._type == MX_YARP_INVALID ||
		myport._direction == true ||
		myport._name == NULL)
		return -1;

	/// can't be #defined because of the many exceptions in handling types.
	switch (myport._type)
	{
		PORT_WRITE(INT, int);
		PORT_WRITE(DOUBLE, double);
		PORT_WRITE(FLOAT, float);

		case MX_YARP_YVECTOR:
		{
			YARPOutputPortOf<YVector> *port = (YARPOutputPortOf<YVector> *)myport._port;
			if (par._sizex != 1 && par._sizey != 1)	/// matrices are not yet allowed.
				return_value = -1;
			else
			{
				/// accepts both row and column vectors.
				if (par._sizex != 1)
				{
					port->Content().Resize (par._sizex);
					memcpy (port->Content().data(), par._extra_content, sizeof(double) * par._sizex);
					port->Write((par._extra_params)?true:false);
					return_value = 0;
				}
				else
				{
					port->Content().Resize (par._sizey);
					memcpy (port->Content().data(), par._extra_content, sizeof(double) * par._sizey);
					port->Write((par._extra_params)?true:false);
					return_value = 0;
				}
			}
		}
		break;

		case MX_YARP_BOTTLE:
		{
			YARPOutputPortOf<YARPBottle> *port = (YARPOutputPortOf<YARPBottle> *)myport._port;

			/// par contains:
			/// the ID of the bottle in _portname.
			/// the number of elements in _sizex.
			/// the array of the bottle in _extra_content.
			/// + the usual params:
			///		_extra_params (blocking flag).
			///
			YARPBottle tmp;
			tmp.reset();
			tmp.setID(par._portname);
			tmp.setDataPtr(par._extra_content, par._sizex);

			port->Content() = tmp;
			port->Write((par._extra_params)?true:false);
			return_value = 0;
		}
		break;

		case MX_YARP_IMAGE:
		{
			YARPOutputPortOf<YARPGenericImage> *port = (YARPOutputPortOf<YARPGenericImage> *)myport._port;

			switch ((par._extra_params & 0x0000ff00) >> 8)
			{
			case YARP_PIXEL_MONO:
				{
					port->Content().SetID (YARP_PIXEL_MONO);
					port->Content().Resize (par._sizex, par._sizey);
					
					int padding = port->Content().GetPadding();
					matlab_to_yarp<unsigned char> ((unsigned char *)par._extra_content, (unsigned char *)port->Content().GetRawBuffer(), par._sizex, par._sizey, padding);

					port->Write((par._extra_params & 0x000000ff)?true:false);
					return_value = 0;
				}
				break;

			case YARP_PIXEL_MONO_SIGNED:
				{
					port->Content().SetID (YARP_PIXEL_MONO_SIGNED);
					port->Content().Resize (par._sizex, par._sizey);
					
					int padding = port->Content().GetPadding();
					matlab_to_yarp<char> ((char *)par._extra_content, (char *)port->Content().GetRawBuffer(), par._sizex, par._sizey, padding);

					port->Write((par._extra_params & 0x000000ff)?true:false);
					return_value = 0;
				}
				break;

			case YARP_PIXEL_INT:
				{
					port->Content().SetID (YARP_PIXEL_INT);
					port->Content().Resize (par._sizex, par._sizey);
					
					int padding = port->Content().GetPadding();
					matlab_to_yarp<int> ((int *)par._extra_content, (int *)port->Content().GetRawBuffer(), par._sizex, par._sizey, padding);

					port->Write((par._extra_params & 0x000000ff)?true:false);
					return_value = 0;
				}
				break;

			case YARP_PIXEL_MONO_FLOAT:
				{
					port->Content().SetID (YARP_PIXEL_MONO_FLOAT);
					port->Content().Resize (par._sizex, par._sizey);
					
					int padding = port->Content().GetPadding();
					matlab_to_yarp<float> ((float *)par._extra_content, (float *)port->Content().GetRawBuffer(), par._sizex, par._sizey, padding);

					port->Write((par._extra_params & 0x000000ff)?true:false);
					return_value = 0;
				}
				break;

			case YARP_PIXEL_RGB:
				{
					port->Content().SetID (YARP_PIXEL_RGB);
					port->Content().Resize (par._sizex, par._sizey);
					
					int padding = port->Content().GetPadding();
					matlab_to_yarp3<unsigned char> ((unsigned char *)par._extra_content, (unsigned char *)port->Content().GetRawBuffer(), par._sizex, par._sizey, padding);

					port->Write((par._extra_params & 0x000000ff)?true:false);
					return_value = 0;
				}
				break;

			case YARP_PIXEL_RGB_SIGNED:
				{
					port->Content().SetID (YARP_PIXEL_RGB_SIGNED);
					port->Content().Resize (par._sizex, par._sizey);
					
					int padding = port->Content().GetPadding();
					matlab_to_yarp3<char> ((char *)par._extra_content, (char *)port->Content().GetRawBuffer(), par._sizex, par._sizey, padding);

					port->Write((par._extra_params & 0x000000ff)?true:false);
					return_value = 0;
				}
				break;

			case YARP_PIXEL_RGB_FLOAT:
				{
					port->Content().SetID (YARP_PIXEL_RGB_SIGNED);
					port->Content().Resize (par._sizex, par._sizey);
					
					int padding = port->Content().GetPadding();
					matlab_to_yarp3<float> ((float *)par._extra_content, (float *)port->Content().GetRawBuffer(), par._sizex, par._sizey, padding);

					port->Write((par._extra_params & 0x000000ff)?true:false);
					return_value = 0;
				}
				break;

			case YARP_PIXEL_INVALID:
				return_value = -1;
				break;
			}
		}
		break;

		default:
			return -1;
			break;
	}

	return return_value;
}


///
///
/// risky if doesn't return from porter. Matlab hangs.
int porter (void *params)
{
	_dispatchParams& par = *(_dispatchParams *)params;
	if (par._portname == NULL || 
		par._extra_content == NULL)
		return -1;

	YARPPort::Connect (par._portname, par._extra_content);

	return 0;
}
