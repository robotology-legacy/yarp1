/*
* smiltime.h -- W3C SMIL2 Time value parser
* Copyright (C) 2003 Dan Dennedy <dan@dennedy.org>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
*/

#ifndef _SMILTIME_H
#define _SMILTIME_H

#include <string>

using namespace std;

namespace SMIL
{

class Time
{
public:
	typedef enum {
	    SMIL_TIME_INDEFINITE = 0,
	    SMIL_TIME_OFFSET,
	    SMIL_TIME_SYNC_BASED,    // not implemented
	    SMIL_TIME_EVENT_BASED,    // not implemented
	    SMIL_TIME_WALLCLOCK,    // not implemented
	    SMIL_TIME_MEDIA_MARKER,    // not implemented
	    SMIL_TIME_REPEAT,     // not implemented
	    SMIL_TIME_ACCESSKEY,    //not implemented
	} TimeType;

protected:
	// internally stored in milliseconds
	long timeValue;
	long offset;
	bool indefinite;
	bool resolved;
	bool syncbaseBegin;
	TimeType timeType;

public:
	Time();
	Time( long time );
	Time( string time );
	virtual ~Time()
	{}

	long getTimeValue()
	{
		return timeValue;
	}
	TimeType getTimeType()
	{
		return timeType;
	}
	long getOffset()
	{
		return offset;
	}

	bool operator< ( Time& time );
	bool operator==( Time& time );
	bool operator> ( Time& time );
	bool isResolved()
	{
		return resolved;
	}
	long getResolvedOffset();
	bool isNegative();
	bool isIndefinite()
	{
		return indefinite;
	}
	virtual string toString();

protected:
	void parseTimeValue( string time );
	long parseClockValue( string time );

private:
	void setTimeValue( Time& source );

};


class MediaClippingTime : public Time
{
public:
	typedef enum {
	    SMIL_SUBFRAME_NONE,
	    SMIL_SUBFRAME_0,
	    SMIL_SUBFRAME_1
	} SubframeType;

	MediaClippingTime( string time, float framerate );
	virtual ~MediaClippingTime()
	{}
	virtual string toString();

private:
	float framerate;
	bool isSmpteValue;
	SubframeType subframe;
	void parseSmpteValue( string time );
};

} // namespace

#endif
