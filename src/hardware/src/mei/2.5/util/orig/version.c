/* VERSION.C

:Reads the hardware, firmware, and FPGA version numbers.

The surface mount controllers have a PROM which contains the on board FPGA logic
 code.  Some of the PROMS/FPGAs support an identification register.

The PROM identification register is mapped into the DSP's external memory map
 at Address 0x0.  The 16 bit value corresponds to a particular PROM version
 and/or option number.

Warning!  This is a sample program to assist in the integration of the
 DSP-Series controller with your application.  It may not contain all 
 of the logic and safety features that your application requires.
  
Written for Library Version 2.5
*/

#include <stdio.h>
#include <stdlib.h>
#include "idsp.h"

#ifdef MEI_MEI_MSVC40
	#include "medexp.h"
#endif

#define BASE_ADDRESS 0x320
static char *board[16] = {
    "Unknown",
    "PCX",
    "STD",
    "104",
    "LC",
    "EXM",
    "VME",
    "Unknown",
    "Unknown",
    "SERCOS",
    "Unknown",
    "Unknown",
    "SERCOS/STD",
    "SERCOS/104",
    "Unknown",
    "Unknown",
};

#define PROM_ID_ADDR		0x0

	
void error(int16 error_code)
{   
	char buffer[MAX_ERROR_LEN];

	switch (error_code)
	{
		case DSP_OK:
			/* No error, so we can ignore it. */
			break ;

		default:
			error_msg(error_code, buffer) ;
			fprintf(stderr, "ERROR: %s (%d).\n", buffer, error_code) ;
			exit(1);
			break;
	}
}

int16 read_prom_id(int16 * identification)
{	int16 prom_id, check;
	long i;

	prom_id = (dsp_read_dm(PROM_ID_ADDR));
	for(i = 0; i < 0xFFFF; i++)		/* make sure id is valid */
	{	dsp_write_dm(PROM_ID_ADDR, (int16)i);
		check = dsp_read_dm(PROM_ID_ADDR);
		if(prom_id != check)
		{	prom_id = 0;	/* identification not supported */	
			break;
		}
	}
	*identification = prom_id;	

	return dsp_error;
}

int main()
{
    int16 i, temp, err, ver, opt, prom_ver, hw_ver, sub_rev, dev;

  err = do_dsp();
  
  
    if(err == DSP_NOT_FOUND)
    {	char buff[MAX_ERROR_LEN];
		error_msg(err, buff);
        printf("initialization error (%d) -- %s\n", err, buff);
        return 1;
    }
    ver = dsp_read_dm(0x10A);
    opt = dsp_read_dm(0x10B);
    dev = opt & 0x8000;
    sub_rev = (opt >> 12) & 7;
    opt &= 0xFFF;

    hw_ver = dsp_read_dm(0x78) & 0xFF;

    for(i = 0; i < 256; i++)
    {
        dsp_write_dm(0x78,i);
        temp = dsp_read_dm(0x78) & 0xFF;
        if(hw_ver != temp)
        {
            hw_ver = -1;
            break;
        }
    }
	read_prom_id(&prom_ver);

    printf("DSP Firmware %s Version: %d.%2.2d, Revision: %c%c, Option: %d\n",
        dev ? "Development" : "Production",
        ver/1600,(ver /16) % 100,
        0x40 + (ver & 15),
        sub_rev ? '0' + sub_rev : ' ',
        opt);

    if(hw_ver > 0)
    {	
		printf("Board Type: %2.2d (%s), Rev: %2.2d\n",
			hw_ver & 15, board[hw_ver &15],hw_ver >> 4);
    }
    else
    {
        printf("Board Type: Undetermined, Rev. Undetermined\n");
    }
    if(prom_ver)
    {
        printf("FPGA Prom Version: %2.2d\n",prom_ver);
    }
    else
    {
        printf("FPGA Prom Version: Undetermined\n");
    }
	return 0;
}
