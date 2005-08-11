/*
* smiltime.cc -- W3C SMIL2 Time value parser
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "smiltime.h"

#include <iostream>
using std::cerr;
using std::endl;
#include <iomanip>
#include <sstream>
using std::ostringstream;

#include <stdlib.h>

#include "stringutils.h"

namespace SMIL
{

Time::Time()
{
	Time( "indefinite" );
}


Time::Time( long time ) :
		timeValue( time ), offset( 0 ),
		indefinite( false ), resolved( true ), syncbaseBegin( false ),
		timeType( SMIL_TIME_OFFSET )
{}


Time::Time( string time )
{
	Time( 0L );
	parseTimeValue( time );
}

void Time::parseTimeValue( string time )
{
	time = StringUtils::stripWhite( time );

	resolved = false;

	if ( StringUtils::begins ( time, "indefinite" ) || time.empty() || time.size() == 0 )
	{
		indefinite = true;
		timeType = SMIL_TIME_INDEFINITE;
		resolved = true;
		//cerr << "smil: this is an indefinite time value" << endl;
	}
	else if ( time.at( 0 ) == '+' || time.at( 0 ) == '-' )
	{
		//cerr << "smil: this is an offset time value" << endl;
		timeValue = parseClockValue( time.substr( 1 ) );
		if ( time.at( 0 ) == '-' )
			timeValue *= -1;
		timeType = SMIL_TIME_OFFSET;
		resolved = true;
	}
	else if ( StringUtils::begins ( time, "wallclock(" ) )
	{
		//parseWallclockValue(time);
		timeType = SMIL_TIME_WALLCLOCK;
		resolved = true;
		//cerr << "smil: this is a wallclock time value" << endl;
	}
	else if ( StringUtils::begins ( time, "accesskey(" ) )
	{
		timeType = SMIL_TIME_ACCESSKEY;
		//cerr << "smil: this is an accesskey	time value" << endl;
	}
	else
	{
		ostringstream token;
		char c;
		string::size_type pos = 0;
		string base;
		for ( ; pos < time.size(); ++pos )
		{
			c = time.at( pos );
			if ( c == '+' || c == '-' )
			{
				string symbol = token.str();
				token.str( string() );
				//cerr << "smil: parsed symbol token '" << symbol << "'" << endl;

				if ( symbol == string( "begin" ) )
				{
					//cerr << "smil: this is a sync based time value" << endl;
					syncbaseBegin = true;
					timeType = SMIL_TIME_SYNC_BASED;
				}
				else if ( symbol == string( "end" ) )
				{
					//cerr << "smil: this is a sync based time value" << endl;
					syncbaseBegin = false;
					timeType = SMIL_TIME_SYNC_BASED;
				}
				else if ( StringUtils::begins( symbol, "marker(" ) )
				{
					//cerr << "smil: this is a media marker time value" << endl;
					//parseMediaMarkerValue(base, token); // base must not be empty
					timeType = SMIL_TIME_MEDIA_MARKER;
				}
				else if ( StringUtils::begins( symbol, "repeat(" ) )
				{
					//cerr << "smil: this is an event repeat time value" << endl;
					//parseRepeatValue(base, token); // base can be empty := current element
					timeType = SMIL_TIME_REPEAT;
				}
				else
				{
					//cerr << "smil: this is an event based time value" << endl;
					//parseEventValue( base, token ); // base can be empty := current element
					timeType = SMIL_TIME_EVENT_BASED;
				}
				offset = parseClockValue( time.substr( pos + 1 ) );
				if ( c == '-' )
					offset *= -1;
				break;
			}
			else if ( c == '.' && ( pos == 0 || time.at( pos - 1 ) != '\\' ) )
			{
				base = token.str();
				token.str( string() );
				//cerr << "smil: parsed base token '" << base << "'" << endl;
			}
			else
			{
				token << c;
			}
		}
		string remaining = token.str();
		//cerr << "smil: parsed remaining token '" << remaining << "'" << endl;
		if ( !remaining.empty() )
		{
			//cerr << "smil: this is an offset time value" << endl;
			offset = parseClockValue( time );
			timeType = SMIL_TIME_OFFSET;
			resolved = true;
		}
	}
}


static inline
string getFraction( string& time )
{
	string::size_type pos;
	string fraction;
	if ( ( pos = time.find( '.' ) ) != string::npos )
	{
		fraction = time.substr( pos );
		//cerr << "smil: fraction = " << fraction << endl;
		time = time.substr( 0, pos );
	}
	return fraction;
}


long Time::parseClockValue( string time )
{
	long total = 0;
	string hours;
	string minutes;
	string seconds;
	string milliseconds;
	string::size_type pos1, pos2;

	if ( ( pos1 = time.find( ':' ) ) != string::npos )
	{
		if ( ( pos2 = time.find( ':', pos1 + 1 ) ) != string::npos )
		{
			//parse Full-clock-value
			//cerr << "smil: parsing Full clock value " << time << endl;
			hours = time.substr( 0, pos1 );
			time = time.substr( pos1 + 1 );
			pos1 = pos2 - pos1 - 1;
		}
		//parse Partial-clock-value
		//cerr << "smil: parsing Partial clock value " << time << endl;
		if ( ( pos2 = time.find( '.' ) ) != string::npos )
		{
			milliseconds = "0" + time.substr( pos2 ); //keep the decimal point
			time = time.substr( 0, pos2 );
		}
		minutes = time.substr( 0, pos1 );
		seconds = time.substr( pos1 + 1 );
	}
	else
	{
		//parse Timecount-value
		//cerr << "smil: parsing Timecount value " << time << endl;
		if ( StringUtils::ends( time, "h" ) )
		{
			total = ( long ) ( atof( getFraction( time ).c_str() ) * 3600000 );
			hours = time;
		}
		else if ( StringUtils::ends( time, "min" ) )
		{
			total = ( long ) ( atof( getFraction( time ).c_str() ) * 60000 );
			minutes = time;
		}
		else if ( StringUtils::ends( time, "ms" ) )
		{
			total = ( long ) ( atof( time.c_str() ) + 0.5 );
		}
		else
		{
			total = ( long ) ( atof( getFraction( time ).c_str() ) * 1000 );
			seconds = time;
		}
	}
	//cerr << "smil: subtotal = " << total << ", h = " << atol(hours.c_str()) <<
	//	", m = " << atol(minutes.c_str()) << ", s = " << atol(seconds.c_str()) <<
	//	", ms = " << (long) (atof(milliseconds.c_str()) * 1000) << endl;
	total += ( atol( hours.c_str() ) * 3600 + atol( minutes.c_str() ) * 60 +
	           atol( seconds.c_str() ) ) * 1000 + ( long ) ( atof( milliseconds.c_str() ) * 1000 );
	return total;
}


long Time::getResolvedOffset()
{
	return ( resolved ? ( timeValue + offset ) : 0 );
}

bool Time::isNegative()
{
	return ( resolved && !indefinite && ( getResolvedOffset() < 0 ) );
}


bool Time::operator< ( Time& time )
{
	return !( ( *this > time ) || ( *this == time ) );
}


bool Time::operator==( Time& time )
{
	return (
	           ( this->isIndefinite() && time.isIndefinite() ) ||
	           ( this->getResolvedOffset() == time.getResolvedOffset() )
	       );
}


bool Time::operator> ( Time& time )
{
	return (
	           ( !resolved ) ||
	           ( indefinite && time.isResolved() && !time.isIndefinite() ) ||
	           ( resolved && time.isResolved() && this->getResolvedOffset() > time.getResolvedOffset() )
	       );
}


void Time::setTimeValue( Time& source )
{
	resolved = source.isResolved();
	indefinite = source.isIndefinite();
	timeValue = source.getTimeValue();
}


string Time::toString()
{
	if ( indefinite )
		return "indefinite";
	else if ( !resolved )
		return "unresolved";
	else
	{
		long ms = getResolvedOffset();
		int hh = ( ms / 3600000 );
		ms -= hh * 3600000;
		int mm = ( ms / 60000 );
		ms -= mm * 60000;
		int ss = ms / 1000;
		ms -= ss;

		ostringstream str;
		str << hh << ":" << mm << ":" << ss << ".";
		str << std::setw( 3 ) << std::setfill( '0' ) << std::ios::right << ms;

		return str.str();
	}
}


MediaClippingTime::MediaClippingTime( string time, float framerate ) :
		Time( 0L ), framerate( framerate ), isSmpteValue( false ), subframe( SMIL_SUBFRAME_NONE )
{
	time = StringUtils::stripWhite( time );
	//cerr << "smil: parsing media clipping time " << time << endl;
	if ( StringUtils::begins( time, "smpte=" ) ||
	        StringUtils::begins( time, "smpte-30-drop=" ) ||
	        StringUtils::begins( time, "smpte-25=" ) )
	{
		parseSmpteValue( time.substr( time.find( '=' ) + 1 ) );
	}
	else if ( time.find( '=' ) != string::npos )    // discard npt=
	{
		parseTimeValue( time.substr( time.find( '=' ) + 1 ) );
	} else
	{
		parseTimeValue( time );
	}
}


void MediaClippingTime::parseSmpteValue( string time )
{
	string hours;
	string minutes;
	string seconds;
	string frames;
	string::size_type pos;

	isSmpteValue = true;

	if ( ( pos = time.find( ':' ) ) != string::npos )
	{
		//cerr << "smil: parsing HH SMPTE value " << time << endl;
		hours = time.substr( 0, pos );
		time = time.substr( pos + 1 );

		if ( ( pos = time.find( ':' ) ) != string::npos )
		{
			//cerr << "smil: parsing MM SMPTE value " << time << endl;
			minutes = time.substr( 0, pos );
			time = time.substr( pos + 1 );

			if ( ( pos = time.find( ':' ) ) != string::npos )
			{
				//cerr << "smil: parsing SS SMPTE value " << time << endl;
				seconds = time.substr( 0, pos );
				time = time.substr( pos + 1 );

				if ( ( pos = time.find( '.' ) ) != string::npos )
				{
					//cerr << "smil: parsing FF SMPTE value " << time << endl;
					frames = time.substr( 0, pos );
					switch ( time.at( pos + 1 ) )
					{
					case '0' :
						subframe = SMIL_SUBFRAME_0;
						break;
					case '1' :
						subframe = SMIL_SUBFRAME_1;
						break;
					default:
						subframe = SMIL_SUBFRAME_NONE;
						break;
					}
				}
				else
				{
					frames = time;
				}
			}
			else
			{
				//cerr << "smil: parsing SS SMPTE value " << time << endl;
				seconds = time;
			}

			//cerr << "hh = " << hours << ", mm = " << minutes << ", ss = " << seconds << ", ff = " << frames << endl;
			timeValue = ( atol( hours.c_str() ) * 3600 + atol( minutes.c_str() ) * 60 +
			              atol( seconds.c_str() ) ) * 1000 + ( long ) ( atof( frames.c_str() ) / framerate * 1000 );

			resolved = true;
		}
		else
			; //throw invalid
	}
	else
		; //throw invalid
}

string MediaClippingTime::toString()
{
	if ( isSmpteValue )
	{
		if ( indefinite )
			return "indefinite";
		else if ( !resolved )
			return "unresolved";
		else
		{
			long ms = getResolvedOffset();
			int hh = ( ms / 3600000 );
			ms -= hh * 3600000;
			int mm = ( ms / 60000 );
			ms -= mm * 60000;
			int ss = ms / 1000;
			ms -= ss;

			ostringstream str;
			if ( framerate == 25.0 )
				str << "smpte-25=";
			else
				str << "smpte=";
			str << hh << ":" << mm << ":" << ss << ":";
			str << ( framerate * ms / 1000.0 );
			if ( subframe == SMIL_SUBFRAME_0 )
				str << ".0";
			else if ( subframe == SMIL_SUBFRAME_1 )
				str << ".1";

			return str.str();
		}
	}
	else
	{
		return Time::toString();
	}
}

} // namespace

