/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialFeedbackData.cpp
 * @brief 
 * @version 1.0
 * @date 26-Jul-06 12:07:39 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: SerialFeedbackData.cpp,v 1.2 2006-07-28 12:39:00 beltran Exp $
 */

#include "SerialFeedbackData.h"

/** 
 * SerialFeedbackData::setSerialResponseDelimiter
 * 
 * @param  
 */
int 
SerialFeedbackData::setSerialResponseDelimiter(char * delimiter)
{
    ACE_NEW_RETURN(_delimiter_string, ACE_String_Base<char>(delimiter),-1);
    _use_string_delimiter      = true;
    _use_bytesnumber_delimiter = false;
    return 0;
}

/** 
 * SerialFeedbackData::setSerialResponseDelimiter
 * 
 * @param number_of_bytes_to_read 
 */
SerialFeedbackData::setSerialResponseDelimiter(int bytes_to_read)
{
    _bytes_to_read = bytes_to_read;
    _use_bytesnumber_delimiter = true;
    _use_string_delimiter      = false;
}

/** 
 * SerialFeedbackData::checkSerialResponseTermination
 * 
 * @param serial_reponse A pointer to the readed data.
 * @param number_of_readed_bytes The number of bytes readed.
 * @return 1 if a end condition has been detected.
 * @return 2 if no end condition has been activated.
 * @return 0 if the end condition has not been meet.
 */
int SerialFeedbackData::checkSerialResponseEnd(char * serial_response, int
    number_of_readed_bytes)
{
    if ( _use_string_delimiter || number_of_readed_bytes == 0 )
    {
        char * result = ACE_OS::strstr( serial_response, _delimiter_string->c_str());
        if (result) return 1;
        else return 0;
    }
    else if ( _use_bytesnumber_delimiter )
    {
        _counted_bytes += number_of_readed_bytes;
        if (_counted_bytes >= _bytes_to_read)
            return 1;
        else return 0;
    }

    return 2; //No termination condition has been activated
}

