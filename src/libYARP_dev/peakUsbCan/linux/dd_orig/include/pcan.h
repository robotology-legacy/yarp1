#ifndef __PCAN_H__
#define __PCAN_H__

//****************************************************************************
// Copyright (C) 2001,2002,2003,2004  PEAK System-Technik GmbH
//
// linux@peak-system.com 
// www.peak-system.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// Maintainer(s): Klaus Hitschler (klaus.hitschler@gmx.de)
//****************************************************************************

//****************************************************************************
//
// pcan.h
// constants and definitions to access the drivers 
//
// $Log: not supported by cvs2svn $
// Revision 1.9  2005/04/07 20:51:02  klaus
// corrected typographic error
//
// Revision 1.8  2004/08/15 09:45:26  klaus
// added PCAN_GET_EXT_STATUS
//
// Revision 1.7  2004/04/11 22:03:29  klaus
// cosmetic changes
//
// Revision 1.6  2004/04/11 22:03:28  klaus
// cosmetic changes
//
// Revision 1.5  2003/03/02 10:58:07  klaus
// merged USB thread into main path
//
// Revision 1.4  2003/03/02 10:58:07  klaus
// merged USB thread into main path
//
// Revision 1.3.2.7  2003/01/29 20:34:20  klaus
// release_20030129_a and release_20030129_u released
//
// Revision 1.3.2.6  2003/01/29 20:34:19  klaus
// release_20030129_a and release_20030129_u released
//
// Revision 1.3.2.5  2003/01/28 23:28:26  klaus
// reorderd pcan_usb.c and pcan_usb_kernel.c, tidied up
//
// Revision 1.3.2.4  2003/01/26 22:35:39  klaus
// it's not allowed to invoke 2 waits for bulk transfer at the same pipe at the same time
//
//****************************************************************************

//****************************************************************************
// INCLUDES
#include <linux/types.h>
#include <linux/ioctl.h>

//****************************************************************************
// DEFINES

//****************************************************************************
// compatibilty defines
#if defined(DWORD) || defined(WORD) || defined(BYTE)
#error "double define for DWORD, WORD, BYTE found"
#endif

#ifdef __KERNEL__
#define DWORD  u32
#define WORD   u16
#define BYTE   u8
#else
#define DWORD  __u32
#define WORD   __u16
#define BYTE   __u8
#endif

//****************************************************************************
// parameter wHardwareType, used by open 
#define HW_ISA             1 // not supported with LINUX, 82C200 chip 
#define HW_DONGLE_SJA      5
#define HW_DONGLE_SJA_EPP  6 
#define HW_DONGLE_PRO      7 // not yet supported with LINUX
#define HW_DONGLE_PRO_EPP  8 // not yet supported with LINUX
#define HW_ISA_SJA         9 // use this also for PC/104
#define HW_PCI		        10 // PCI carries always SJA1000 chips
#define HW_USB            11 // don't know if this is common over peak products

//****************************************************************************
// mask for standard and extended CAN identifiers
#define CAN_MAX_STANDARD_ID     0x7ff
#define CAN_MAX_EXTENDED_ID     0x1fffffff

//****************************************************************************
// error codes
#define CAN_ERR_OK             0x0000  // no error
#define CAN_ERR_XMTFULL        0x0001  // transmit buffer full
#define CAN_ERR_OVERRUN        0x0002  // overrun in receive buffer
#define CAN_ERR_BUSLIGHT       0x0004  // bus error, errorcounter limit reached
#define CAN_ERR_BUSHEAVY       0x0008  // bus error, errorcounter limit reached
#define CAN_ERR_BUSOFF         0x0010  // bus error, 'bus off' state entered
#define CAN_ERR_QRCVEMPTY      0x0020  // receive queue is empty
#define CAN_ERR_QOVERRUN       0x0040  // receive queue overrun
#define CAN_ERR_QXMTFULL       0x0080  // transmit queue full 
#define CAN_ERR_REGTEST        0x0100  // test of controller registers failed
#define CAN_ERR_NOVXD          0x0200  // Win95/98/ME only
#define CAN_ERR_RESOURCE       0x2000  // can't create resource
#define CAN_ERR_ILLPARAMTYPE   0x4000  // illegal parameter
#define CAN_ERR_ILLPARAMVAL    0x8000  // value out of range
#define CAN_ERRMASK_ILLHANDLE  0x1C00  // wrong handle, handle error

//****************************************************************************
// MSGTYPE bits of element MSGTYPE in structure TPCANMsg
#define MSGTYPE_STATUS        0x80     // used to mark pending status
#define MSGTYPE_EXTENDED      0x02     // declares a extended frame
#define MSGTYPE_RTR           0x01     // marks a remote frame
#define MSGTYPE_STANDARD      0x00     // marks a standard frame

//****************************************************************************
// maximum length of the version string (attention: used in driver too)
#define VERSIONSTRING_LEN     64  

//****************************************************************************
// structures to communicate via ioctls
typedef struct
{
  WORD wBTR0BTR1;        // merged BTR0 and BTR1 register of the SJA1000
  BYTE ucCANMsgType;     // 11 or 29 bits - put MSGTYPE_... in here
  BYTE ucListenOnly;     // listen only mode when != 0
} TPCANInit;             // for PCAN_INIT

typedef struct 
{
  DWORD ID;              // 11/29 bit code
  BYTE  MSGTYPE;         // bits of MSGTYPE_*
  BYTE  LEN;             // count of data bytes (0..8)
  BYTE  DATA[8];         // data bytes, up to 8
} TPCANMsg;              // for PCAN_WRITE_MSG

typedef struct
{
  TPCANMsg Msg;          // the above message
  DWORD    dwTime;       // a timestamp in msec, read only
} TPCANRdMsg;            // for PCAN_READ_MSG

typedef struct 
{
  WORD  wErrorFlag;      // same as in TPDIAG, is cleared in driver after access
  int   nLastError;      // is cleared in driver after access
} TPSTATUS;              // for PCAN_GET_STATUS

typedef struct
{
  WORD  wType;           // the type of interface hardware - see HW_....
  DWORD dwBase;          // the base address or port of this device
  WORD  wIrqLevel;       // the irq level of this device
  DWORD dwReadCounter;   // counts all reads to this device from start
  DWORD dwWriteCounter;  // counts all writes
  DWORD dwIRQcounter;    // counts all interrupts
  DWORD dwErrorCounter;  // counts all errors
  WORD  wErrorFlag;      // gathers all errors
  int   nLastError;      // the last local error for this device
  int   nOpenPaths;      // number of open paths for this device
  char  szVersionString[VERSIONSTRING_LEN]; // driver version string
} TPDIAG;                // for PCAN_DIAG, in opposition to PCAN_GET_STATUS nothing is cleared
  
typedef struct
{
	DWORD dwBitRate;       // in + out, bitrate in bits per second
	WORD  wBTR0BTR1;       // out only: the result
} TPBTR0BTR1;

typedef struct 
{
  WORD  wErrorFlag;      // same as in TPDIAG, is cleared in driver after access
  int   nLastError;      // is cleared in driver after access
  int   nPendingReads;   // count of unread telegrams
  int   nPendingWrites;  // count of unsent telegrams
} TPEXTENDEDSTATUS;      // for PCAN_GET_ESTATUS

//****************************************************************************
// some predefines for ioctls
#define PCAN_MAGIC_NUMBER  'z'
#define MYSEQ_START        0x80

//****************************************************************************
// ioctls control codes
#define PCAN_INIT           _IOWR(PCAN_MAGIC_NUMBER, MYSEQ_START,     TPCANInit)
#define PCAN_WRITE_MSG      _IOW (PCAN_MAGIC_NUMBER, MYSEQ_START + 1, TPCANMsg)
#define PCAN_READ_MSG       _IOR (PCAN_MAGIC_NUMBER, MYSEQ_START + 2, TPCANRdMsg)
#define PCAN_GET_STATUS     _IOR (PCAN_MAGIC_NUMBER, MYSEQ_START + 3, TPSTATUS)
#define PCAN_DIAG           _IOR (PCAN_MAGIC_NUMBER, MYSEQ_START + 4, TPDIAG)
#define PCAN_BTR0BTR1       _IOWR(PCAN_MAGIC_NUMBER, MYSEQ_START + 5, TPBTR0BTR1)
#define PCAN_GET_EXT_STATUS _IOR (PCAN_MAGIC_NUMBER, MYSEQ_START + 6, TPEXTENDEDSTATUS)

#endif // __PCAN_H__
