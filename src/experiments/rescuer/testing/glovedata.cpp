/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file glovedata.c
 * @brief 
 * @version 1.0
 * @date 01-Aug-06 10:38:13 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: glovedata.cpp,v 1.2 2006-08-02 15:30:37 beltran Exp $
 */
#include "glovedata.h"

void 
DataGloveData::dump()
{
    printf("*****Data Glove Structure*****\n");
    printf("Thumb: %d %d %d \n", thumb[0],thumb[1],thumb[2]);
    printf("PalmArch: %d \n",palmArch);
    printf("WristPitch: %d\n",wristPitch);
    printf("WristYaw: %d\n", wristYaw);
    printf("*****************************\n");
}
size_t DataGloveData::getMessageLength() const
{
    return ACE_OS::strlen(finalmessage);
}

