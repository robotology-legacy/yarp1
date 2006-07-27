/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSErrorCodes.h
 * @brief 
 * @version 1.0
 * @date 27-Jul-06 8:43:43 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGSErrorCodes.h,v 1.1 2006-07-27 18:47:02 beltran Exp $
 */

#ifndef __DGSERRORCODESH__
#define __DGSERRORCODESH__

/**
 * \file DGSErrorCodes.h contains definitions of the DGS return types.
 */

/**
 * Generic Dexterous Grasping System error codes.
 */
enum DGSErrorCodes
{
  /**
   * A DGS function succeeded.
   */
	DGS_OK = 0,

  /**
   * A DGS function failed.
   */
	DGS_FAIL = -1,
};

#endif

