/* */

#include "DSP807_serial.h"

void DSP_SendUWord16AsChars (UWord16 data)
{
	UWord16 lsb;
	UWord16 buf;
	char c[4];
	int i;

	buf = data;
	for(i=3;i>=0;i--)
	{
		lsb = 0x000F & buf;
		c[i] = DSP_HexToChar(lsb);
		buf = __shr(buf, 4);
	}
	DSP_SendData(c, 4);	
}

void DSP_SendWord16AsChars (Word16 data)
{
	UWord16 lsb;
	Word16 buf;
	UWord16 sign;
	char c[5];
	int i;

	sign = 0x8000 & data;
	
	if (sign != 0)
	{
		buf = __negate(data);
		c[0] = '-';
	}
	else
	{
		buf = 0x7FFF & data;
		c[0] = '+';
	}
		
	for(i=4;i>=1;i--)
	{
		lsb = 0x000F & buf;
		c[i] = DSP_HexToChar(lsb);
		buf = __shr(buf, 4);
	}
		
	DSP_SendData(c, 5 );	
}

char DSP_HexToChar (byte value)
{
	char c;
	switch(value)
	{
		case 0:
			c = '0';
			break;
		case 1:
			c = '1';
			break;
		case 2:
			c = '2';
			break;
		case 3:
			c = '3';
			break;
		case 4:
			c = '4';
			break;
		case 5:
			c = '5';
			break;
		case 6:
			c = '6';
			break;
		case 7:
			c = '7';
			break;
		case 8:
			c = '8';
			break;
		case 9:
			c = '9';
			break;
		case 10:
			c = 'a';
			break;
		case 11:
			c = 'b';
			break;
		case 12:
			c = 'c';
			break;
		case 13:
			c = 'd';
			break;
		case 14:
			c = 'e';
			break;
		case 15:
			c = 'f';
			break;
		default:
			c = '*';
			break;
	}
	return c;
}

#define _XSIZE 255
#define DSP_atoiEx (s) DSP_atoi (s, DSP_strlen(s, _XSIZE));

int DSP_atoi (char *strAddr, int strSize)
{
	byte i, end;
	int val = 0;
	int _pow = 1;
	byte negative = 0;

	if (strSize > 255)
		return 0;
		
	if (strAddr[0] == '-')
	{
		negative = 1;
		end = 1;
	}
	else
	if (strAddr[0] == '+')
	{
		negative = 0;
		end = 1;
	}
	else
		end = 0;

	for (i = strSize-1; (i >= end && strAddr[i] != 0); i--)
	{
		if (strAddr[i] < 48 || strAddr[i] > 57)
			return 0;
		val += _pow * (strAddr[i] - 48);
		_pow *= 10;
	}
	
	if (negative == 1)
		return -val;
	else
		return val;
}


long DSP_atol (char *strAddr, int strSize)
{
	byte i, end;
	long val = 0;
	long _pow = 1;
	byte negative = 0;

	if (strSize > 255)
		return 0;
		
	if (strAddr[0] == '-')
	{
		negative = 1;
		end = 1;
	}
	else
	if (strAddr[0] == '+')
	{
		negative = 0;
		end = 1;
	}
	else
		end = 0;

	for (i = strSize-1; (i >= end && strAddr[i] != 0); i--)
	{
		if (strAddr[i] < 48 || strAddr[i] > 57)
			return 0;
		val += (_pow * (strAddr[i] - 48));
		_pow *= 10;
	}
	
	if (negative == 1)
		return -val;
	else
		return val;
}


int DSP_strlen (char *strAddr, int strSize)
{
	int i=0;
	while ( (strAddr[i] != '\0') && (i<strSize) )
	{
		i++;
	}
	return i;
}

void DSP_SendData (char *dataAddress, int dataSize)
{
	int i;
	int charsInBuf = 0;
	for (i=0;i<dataSize;i++)
	{
		do
  			charsInBuf = AS1_GetCharsInTxBuf();
  		while (charsInBuf > 0);
		AS1_SendChar(dataAddress[i]);
	}
}

void DSP_SendDataEx (char *dataAddress)
{
	int i = 0;
	int charsInBuf = 0;
	
	if (dataAddress == NULL)
		return;
		
	while (dataAddress[i] != 0)
	{
		do
		{
  			charsInBuf = AS1_GetCharsInTxBuf();
  		}
  		while (charsInBuf > 0);
  		
		AS1_SendChar(dataAddress[i]);
		i++;
	}
}

void DSP_ReceiveData (char *dataAddress, int dataSize)
{
	int i;
	int readChars;
	unsigned char c;
	for (i = 0; i < dataSize; i++)
	{
		do
  			readChars = AS1_GetCharsInRxBuf();
  		while (readChars == 0);
  		
		AS1_RecvChar(&c);
		dataAddress[i] = c;
	}
}

int DSP_ReceiveDataEx (char *dataAddress, int dataSize, bool echo)
{
	int i = 0;
	int readChars = 0;
	unsigned char c = 0;
	
	if (dataAddress == NULL || dataSize <= 0)
		return 0;
	
	do
	{
		do
  			readChars = AS1_GetCharsInRxBuf();
  		while (readChars == 0);
	
		AS1_RecvChar(&c);
		if (echo && c >= 32)
			AS1_SendChar(c);
		dataAddress[i] = c;
		i++;
	}
	while (c != '\r' && i < dataSize-1);

	if (echo)
		DSP_SendDataEx ("\r\n");
	
	if (c == '\r')
		i--;

	dataAddress[i] = 0;	
	return (i > 0) ? i : 0;
}

void DSP_SendDWordAsChars (dword data)
{
	dword dwrd;
	word wrd;
	word buf;
	char c[8];
	int i;

	dwrd = 0x0000FFFF & data;
	wrd = dwrd;
	for(i=7;i>=4;i--)
	{
		buf = 0x0000000F & wrd;
		c[i] = DSP_HexToChar(buf);
		wrd = __shr(wrd, 4);
	}
	
	dwrd = 0xFFFF0000 & data;
	wrd = dwrd>>16;
	
	for(i=3;i>=0;i--)
	{
		buf = 0x0000000F & wrd;
		c[i] = DSP_HexToChar(buf);
		wrd = __shr(wrd, 4);
	}
	
	DSP_SendData(c, sizeof(c) );	
}

void DSP_SendDWordAsCharsDec (dword data)
{
	dword dwrd;
	dword sign;
	char c[10];
	int i;
		
	sign = 0x80000000 & data;
	if (sign)
	{
		data = -data;
		c[0] = '-';
	}
	else
		c[0] = '+';
	
	for (i = 0; i < 9; i++)
	{
		dwrd = data % 10;
		data /= 10;
		c[9-i] = dwrd + 48;
	}

	DSP_SendData (c, 10);	
}

void DSP_SendByteAsChars (byte data)
{
	byte bty;
	byte buf;
	char c[8];
	int i;
	bty = data;
	for(i=7;i>=0;i--)
	{
		buf = 00000001 & bty;
		c[i] = DSP_HexToChar(buf);
		bty = __shr(bty, 1);
	}
	
	DSP_SendData(c, sizeof(c) );
}

/*
void lkne(char *dataAddress, int maxSize)
{
	int readedChars = 0;
	int i = 0;
	unsigned char c;
	
	do
	{
		do
  			readedChars = AS1_GetCharsInRxBuf();
  		while (readedChars == 0);
		AS1_RecvChar(&c);
		if ( c != '\r')
			dataAddress[i] = c;
		i++;
	} while ( (i<maxSize) && (c != '\r') );
	dataAddress[i-1] = '\0';
}
*/