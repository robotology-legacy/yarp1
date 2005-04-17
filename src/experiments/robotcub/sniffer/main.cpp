///
/// simple sniffer code for valuecan. writes to file.
///	 by pasa.
///

#include <stdio.h>
#include <windows.h>
#include <icsnVC40.h>

///
///
///

icsSpyMessage stMessages[19999];
int iDeviceTypes[255];
int iComPort[255];
int iSerialNum[255];

///
///
///

int main()
{
	FILE *fp;

	int hObject = 0;				
	int lNumberOfMessages = 0;		
	int lNumberOfErrors = 0;		
	int iNumDevices;
	unsigned long lResult;
	unsigned char bNetworkID[16];
	int lCount;

	for (lCount = 0; lCount < 16; lCount++)
		bNetworkID[lCount] = lCount;

	if (icsneoFindAllCOMDevices(INTREPIDCS_DRIVER_STANDARD, 0, 1, 1, iDeviceTypes, iComPort, iSerialNum, &iNumDevices))
	{
		Sleep(200);

        if (iNumDevices > 0)
        {
			lResult = icsneoOpenPortEx(iComPort[0] ,NEOVI_COMMTYPE_RS232,  
									INTREPIDCS_DRIVER_STANDARD, 0, 57600, 1, bNetworkID, &hObject);
			if (lResult == 0)
			{
				printf ("problems opening port\n");
				return -1;
			}
        }
		else
		{
			printf ("no devices found\n");
			return -1;
		}
	}
	else
	{
		printf ("problems finding ports/devices\n");
		return -1;
	}

	bool finished = false;
	fp = fopen ("log.txt", "w");
	if (fp == NULL)
	{
		printf("can't open log file\n");
		return -1;
	}

	int totalcount = 0;
	double dTime;
	int iTime;
	int count = 0;

	while (1)
	{
		lResult = icsneoGetMessages(hObject, stMessages, &lNumberOfMessages, &lNumberOfErrors);
		if (lResult == 0)
		{
			printf ("troubles reading from device\n");
		}
		else
		{
			printf ("read %d messages\n", lNumberOfMessages);
			fprintf (fp, "read %d messages, and %d errors\n", lNumberOfMessages, lNumberOfErrors);			
			totalcount += lNumberOfMessages;

			for (lCount = 0; lCount < lNumberOfMessages; lCount++)
			{
				if (stMessages[lCount].StatusBitField & SPY_STATUS_GLOBAL_ERR)
				{
					printf ("error\n\n");
				}

				// Calculate the time for this message
				dTime = ((double) stMessages[lCount].TimeHardware2) * NEOVI_TIMESTAMP_2 + 
									((double) stMessages[lCount].TimeHardware) * NEOVI_TIMESTAMP_1;
				dTime *= 1000;				// convert to milli-seconds
				iTime = (int) dTime;		// conever to an integer

				// is the current message a CAN message
				if (stMessages[lCount].Protocol == SPY_PROTOCOL_CAN && stMessages[lCount].ArbIDOrHeader < 0xff)
				{
					// copy a can style message
					fprintf (fp, "Time (ms) %d Message %d Network %d ArbID %X Data Length: %d b1: %X b2: %X b3: %X b4: %X b5: %X b6: %X b7: %X b8: %X\n", 
						iTime, 
						lCount, 
						stMessages[lCount].NetworkID, 
						stMessages[lCount].ArbIDOrHeader,
						stMessages[lCount].NumberBytesData,
						stMessages[lCount].Data[0],
						stMessages[lCount].Data[1],
						stMessages[lCount].Data[2],
						stMessages[lCount].Data[3],
						stMessages[lCount].Data[4],
						stMessages[lCount].Data[5],
						stMessages[lCount].Data[6],
						stMessages[lCount].Data[7]);

					fflush (fp);
				}
			}
		}

		count ++; 

		if (count == 15)
		{
			for (int k = 0; k < 12; k++)
			{
				icsSpyMessage x;
				x.NetworkID = 1;
				x.ArbIDOrHeader = 15-k/2;
				x.NumberBytesData = 1;
				x.StatusBitField = 0;	// not extended or remote frame
				x.StatusBitField2 = 0;

				if ((k % 2) == 0)
					x.Data[0] = 0x14;
				else
					x.Data[0] = 0x94;

				lResult = icsneoTxMessages(hObject, &x, NETID_HSCAN, 1);
				if (lResult == 0) 
					printf ("problems transmitting data\n");
			}

			count = 0;
		}

		Sleep (10);
	}

	icsneoClosePort(hObject, &lNumberOfErrors);
	icsneoFreeObject(hObject);

	fclose (fp);

	return 0;
}

