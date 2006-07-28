/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialFeedbackData.h
 * @brief Implementation of SerialFeedbackData class
 * @version 1.0
 * @date 26-Jul-06 12:16:12 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: SerialFeedbackData.h,v 1.2 2006-07-28 12:39:00 beltran Exp $
 */

#ifndef _SERIALFEEDBACKDATAH_ 
#define _SERIALFEEDBACKDATAH_

#include <ace/OS.h>
#include <ace/String_Base.h>
#include "DGSTask.h"

/** 
 * @class SerialFeedbackData
 * @brief Contains data used by the Serial Handler to detect the end of a serial
 * command and send the response back to the command sender.
 */
class SerialFeedbackData
{

public:

    SerialFeedbackData ()
    {
        // Initialize local parameters.
        _delimiter_string = 0;
        _command_sender   = 0;
        _bytes_to_read    = 0;
        _counted_bytes    = 0;
        _use_string_delimiter      = false;
        _use_bytesnumber_delimiter = false;
    };  /* constructor */

    ~SerialFeedbackData()
    {
        if ( _delimiter_string != 0) free(_delimiter_string);
    };

    /** 
     * setCommandSender 
     * 
     * @param command_sender The pointer to the Task that has sent the serial
     * command.  
     */
    void setCommandSender( DGSTask * command_sender){ _command_sender = command_sender; };

    /** 
     * getCommandSender
     * 
     * @return 
     */
    DGSTask * getCommandSender(){ return _command_sender;};

    /** 
     * setSerialResponseDelimiter
     * 
     * @param delimiter It is a string that should contain a caractheristic
     * reponse of the serial device indicating the end of a response. E.g. The
     * Barrett Hand issues a "=>" when it is expecting a new command. This
     * special string can be used to detect the end of a command response. This
     * response finalization method inhibits the use of a byte count to detect
     * the end of the response. 
     *
     * @return -1 Proble in memory reservation.
     * 0 All ok
     */
     int setSerialResponseDelimiter(char * delimiter);

    /** 
     * Sets the numbers of bytes that are expected in the response.
     * 
     * @param number_of_bytes It the number of bytes that the Command senders
     * expect to read from the delivered command. The activation of this
     * modality deactivates the use of a string to detect the end of a response
     * command.
     */
    setSerialResponseDelimiter(int bytes_to_read); 


    /** 
     * Checks is the serial response has finish.
     * If data is still ExpectedLength
     *
     * @return false if data is still expected
     * true if the serial finalization condition has been detected.
     *
     */
    int checkSerialResponseEnd(char * serial_response, int 
        number_of_readed_bytes = 0);
protected:

private:
    DGSTask * _command_sender;      /// Pointer to the DGSTask that has sent the command.
    ACE_String_Base<char> * _delimiter_string;   /// The delimiter string that detects the end of a serial response.
    int _bytes_to_read;             /// The number of bytes that are expected to be read for a response.
    bool _use_string_delimiter;      /// Determines whether or not use the string
                                    /// delimiter to detect the end of a response
    bool _use_bytesnumber_delimiter; /// Determines whether or not use a count of
                                    /// readed bytes to control the reponse limit.
    int _counted_bytes; /// A counter tha acculates the number of bytes readed so far.
}; 


#endif

