/* CONFIG.C

:Utility to test memory, download/upload firmware, and configure DAC offsets.

Command line switches:

  -f [filename]		configure controller with a particular firmware file
  -d [filename]		download firmware file only
  -u [filename]		upload firmware file only
  -b [base]			set base address
  -a [axes]			configure 'n' number of axes
  -v				verbose, all messages displayed
  -w				no warning message

Warning!  This is a sample program to assist in the integration of the
 DSP-Series controller with your application.  It may not contain all 
 of the logic and safety features that your application requires.
  
Written for Library Version 2.5  
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "idsp.h"

#define	CONFIG_VERSION			"4.0"

#ifdef __QNX__
#  include <conio.h>
#endif

#ifdef EPC
#  include "busmgr.h"
#endif

#ifdef XVME
#  include "xvmedefs.h"
#  include "vmeext.h"
#endif

#ifdef MEI_WINNT
#	include <winbase.h>
#	include <winioctl.h>
#endif

#ifdef MEI_MSVC40
#	include "medexp.h"
#endif

#ifdef MEI_OS9
#	define STRING_CMP_NC(a, b)		strcmp(a, b)
#	define STRING_CMP_PARTIAL(a, b, length) strncmp(a, b, length)
#endif   /* MEI_OS9 */

#ifndef STRING_CMP_NC	/* compare two strings without case sensitivity */
#	define STRING_CMP_NC(a, b)		stricmp(a, b)
#endif

#ifndef STRING_CMP_PARTIAL /* compare two stings without case sensitivity */
#	define STRING_CMP_PARTIAL(a, b, length)	strnicmp(a, b, length)
#endif

#define	CONFIG_MODE		0	/* download and configure firmware */
#define DOWN_MODE		1	/* download firmware only */
#define UP_MODE			2	/* upload firmware only */

char	*FirmwareFile = "8axis.abs",
		*UserFirmwareFile = "8axis.abs";
int16	verbose = 0,
		warning = 1,
		dm_0_location = 0x101,
		dm_0_value = 0x641C,
		partial_failure = 0,
		busted_bits = 0,
		initial_axes = PCDSP_MAX_AXES,
		firmware_mode = CONFIG_MODE,
		user_axes = 0,
		base_addr = 0;
char	dummy = (char)0xFF;


enum
{	EC_OUT_OF_MEMORY = 0x100,
	EC_CANT_OPEN,
	EC_WARNING_MSG,
	EC_TOO_MUCH_OFFSET,
	EC_FIRMWARE_DOWNLOADED,
	EC_FIRMWARE_UPLOADED,
	EC_AXIS_PATHETIC,
	EC_MOTION_TEST,
	EC_MOTION_FAILED,
	EC_MOTION_OK,
	EC_CONFIG_VERSION,
	EC_FIRMWARE_VERSION,
	EC_RESET,
	EC_NT_BASE,
	EC_PAGE_0_VERIFY,
	EC_PAGE_2_VERIFY,
	EC_ILLEGAL_ARGUMENT,
	EC_TESTING_DM,
	EC_DM_FAILURE,			
	EC_DM_1,
	EC_DM_1_FAILURE,
	EC_DM_1_FAILED,
	EC_DM_2,
	EC_DM_2_FAILURE,
	EC_DM_2_FAILED,
	EC_DM_3,
	EC_DM_3_FAILURE,
	EC_DM_3_FAILED,
	EC_DM_4,
	EC_DM_4_FAILURE,
	EC_DM_4_FAILED,
	EC_DM_ADDRESS,
	EC_DM_DATA,
	EC_DM_ONLY,
	EC_DM_BOOT_ADDRESS,
	EC_PATHETIC,
};

double MotionFinal[] = { 500.0, -500.0 };
double MotionVelocity = 1000.0;
double MotionAccel = 100000.0;

int16 Points = sizeof(MotionFinal) / sizeof(MotionFinal[0]);

#define	CUSHION				(6)	/* allow a few seconds longer for a move. */
#define	OFFSET_RANGE		2048
#define	O_VERIFY_BOOT


int16 Error(int16 ec, ...)
{	va_list args;
	va_start(args, ec);

	switch(ec)
	{	case DSP_OK:
			break;

		case EC_CANT_OPEN:
			fprintf(stderr, "Can't open file: %s\n", va_arg(args, char*));
			exit(1);
			break;

		case EC_WARNING_MSG:
			fprintf(stderr, "\nWARNING: The analog and step/dir outputs will now be tested.\n"
					"Make sure all motor axis cables are disconnected from the controller.\n"
					"(Press any key to continue, ESC to quit.)\n");
		#ifdef MEI_OS9
			if (getc(stdin) == 0x1B)
				exit(1);
		#else
			if (getch() == 0x1B)
				exit(1);
		#endif
			break;

		case EC_OUT_OF_MEMORY:
			fprintf(stderr, "Out of memory!\n");
			exit(1);
			break;

		case EC_TOO_MUCH_OFFSET:
		{	int16 axis = va_arg(args, int16), offset = va_arg(args, int16);
			fprintf(stderr, "Axis %d's offset has gone out of range (%d).\n", axis, offset);
			return 1;
		}	

		case EC_FIRMWARE_DOWNLOADED:
			printf("Downloaded firmware from %s.\n", va_arg(args, char*));
			break;
		
		case EC_FIRMWARE_UPLOADED:
			printf("Uploaded firmware to %s.\n", va_arg(args, char*));
			break;

		case EC_AXIS_PATHETIC:
			printf("(axis not found!)\n");
			break;

		case EC_MOTION_TEST:
			printf("Motion Test: ");
			break;

		case EC_MOTION_FAILED:
			printf("\nMotion Test Failed!\n");
			return 1;

		case EC_MOTION_OK:
			printf("\nMotion Test Passed!\n");
			return 0;

		case EC_CONFIG_VERSION:
			printf("CONFIG version %s, DSP-Series library version %s.\n",
					CONFIG_VERSION, PCDSP_ASCII_VERSION);
			break;
	
		case EC_FIRMWARE_VERSION:
		{	int16 ver, opt, dev, sub_rev;

			ver = dsp_read_dm(DM_FIRMWARE_VERSION);
			opt = dsp_read_dm(DM_OPTION);
			dev = opt & 0x8000;
			sub_rev = (opt >> 12) & 7;
			opt &= 0xFFF;

			printf("DSP Firmware %s Version: %d.%2.2d, Revision: %c%c, Option: %d\n",
				dev ? "Development" : "Production",
				ver/1600,(ver /16) % 100,
				0x40 + (ver & 15),
				sub_rev ? '0' + sub_rev : ' ',
				opt);
		}	break;

		case EC_RESET:
		{	int16 e = va_arg(args, int16);
			if(e)
			{	printf("Reset: ERROR (%d)\n", e);
				exit(1);
			}
		}	break;

		case EC_PAGE_0_VERIFY:
			printf("Boot memory page 0 verification error!\n");
			break;

		case EC_PAGE_2_VERIFY:
			printf("Boot memory page 2 verification error!\n");
			break;

		case EC_ILLEGAL_ARGUMENT:
			fprintf(stderr, "Illegal command-line option: %s\n", va_arg(args, char*));
			fprintf(stderr, "Command line options:\n"
				" -f [filename]   configure controller with firmware file\n"
				" -d [filename]   download firmware file only\n"
				" -u [filename]   upload firmware file only\n"
				" -b [base]       set base address\n"
				" -a [axes]       configure 'n' number of axes\n"
				" -v              verbose, all messages displayed\n"
				" -w              no warning message\n");
			exit(1);
			break;

		case EC_TESTING_DM:
			printf("Testing data memory.\n");
			break;

		case EC_DM_1:
			printf("Part 1:  Testing data memory with sequential values.\n");
			break;
		case EC_DM_1_FAILURE:
		{	int16 a = va_arg(args, int16),
					r = va_arg(args, int16),
					w = va_arg(args, int16);
			if(verbose)
				printf("0x%4.4X: wrote 0x%4.4X, read 0x%4.4X  (xor: 0x%4.4X, 0x%4.4X broken).\n",
						a, w, r, w ^ r, busted_bits);
		}	break;
		case EC_DM_1_FAILED:
			printf("Sequential read/write of data memory failed.\n");
			break;
		case EC_DM_2:
			printf("Part 2:  Testing data memory with random values.\n");
			break;
		case EC_DM_2_FAILURE:
		{	int16 a = va_arg(args, int16),
				r = va_arg(args, int16),
				w = va_arg(args, int16);
			if(verbose)
				printf("0x%4.4X: wrote 0x%4.4X, read 0x%4.4X  (xor: 0x%4.4X, 0x%4.4X broken).\n",
						a, w, r, w ^ r, busted_bits);
		}	break;
		case EC_DM_2_FAILED:
			printf("Random read/write of data memory failed.\n");
			break;
		case EC_DM_3:
			printf("Part 3:  Testing boot memory with linear values.\n");
			break;
		case EC_DM_3_FAILURE:
		{	int16 a = va_arg(args, int16),
					r = va_arg(args, int16),
					w = va_arg(args, int16);
			if(verbose)
				printf("0x%4.4X: wrote 0x%2.2X, read 0x%2.2X  (xor: 0x%2.2X, 0x%4.4X broken).\n",
						a, w, r, w ^ r, busted_bits);
		}	break;
		case EC_DM_3_FAILED:
			printf("Linear read/write of boot memory failed.\n");
			break;
		case EC_DM_4:
			printf("Part 4:  Testing boot memory with random values.\n");
			break;
		case EC_DM_4_FAILURE:
		{	int16 a = va_arg(args, int16),
					r = va_arg(args, int16),
					w = va_arg(args, int16);
			if(verbose)
				printf("0x%4.4X: wrote 0x%2.2X, read 0x%2.2X  (xor: 0x%2.2X, 0x%4.4X broken).\n",
						a, w, r, w ^ r, busted_bits);
		}	break;
		case EC_DM_4_FAILED:
			printf("Random read/write of boot memory failed.\n");
			break;
		case EC_DM_FAILURE:
		{	printf("Data memory testing failure.\n");
			if(!partial_failure)
			{	printf("Unable to communicate with the controller at all."
				"  Be sure to check\nthe base I/O address jumpers, or for"
				"device I/O base address conflicts.\n\n");
			}
			else
			{	printf("Intermittent communciation failure--be sure to"
						" check for I/O base address conflicts.\n");
				printf("For specific information about communication failure,\n"
						" try using the '-v' option.\n\n");
			}
			exit(1);
		}	break;
		case EC_DM_ADDRESS:
			printf("There appears to be an on-board addressing problem,\n"
					"address lines 0x%4.4X.\n", va_arg(args, int16));
			break;
		case EC_DM_DATA:
			printf("There appears to be an on-board data signal problem,\n"
					"data lines 0x%4.4X.\n", va_arg(args, int16));
			break;
		case EC_DM_ONLY:
			printf("This problem only appears in data memory, boot memory\n"
				"appears to work properly.\n\n");
			break;
		case EC_DM_BOOT_ADDRESS:
			printf("Boot memory appears to have an addressing problem,\n"
				"address lines 0x%4.4X.\n\n", va_arg(args, int16));
			break;
		case EC_PATHETIC:
			printf("We can't access the controller--is the I/O base address\n"
					"set properly, or is there any device I/O port conflicts?\n\n");
			break;
		default:
			fprintf(stderr, "DSP Error %d:\n", ec);
			fprintf(stderr, "  ERROR\n");
			exit(1);
	}

	return 0;
}


int16 Arguments(int argc, char * argv[])
{	int16 i;
	char * s;

	for(i = 1; i < argc; i++)
	{	s = argv[i];
		if(!STRING_CMP_NC(s, "-v"))		/* verbose */
		{	verbose++;
			continue;
		}
		if(!STRING_CMP_NC(s, "-w"))		/* no warnings */
		{	warning = 0;
			continue;
		}
		if(!STRING_CMP_PARTIAL(s, "-f", 2))		/* firmware file, configure */
		{	if(strlen(s) > 2)
			{	UserFirmwareFile = s + 2;
				continue;
			}
			if(i + 1 == argc)
			{	Error(EC_ILLEGAL_ARGUMENT, argv[i]);
			}
			UserFirmwareFile = argv[++i];
			if(s[1] == 'd')
			{	firmware_mode = CONFIG_MODE;
			}
			continue;
		}
		if(!STRING_CMP_PARTIAL(s, "-d", 2))		/* firmware file, download only */
		{	if(strlen(s) > 2)
			{	UserFirmwareFile = s + 2;
				continue;
			}
			if(i + 1 == argc)
			{	Error(EC_ILLEGAL_ARGUMENT, argv[i]);
			}
			UserFirmwareFile = argv[++i];
			if(s[1] == 'd')
			{	firmware_mode = DOWN_MODE;
			}
			continue;
		}
		if(!STRING_CMP_PARTIAL(s, "-u", 2))		/* firmware file, upload */
		{	if(strlen(s) > 2)
			{	UserFirmwareFile = s + 2;
				continue;
			}
			if(i + 1 == argc)
			{	Error(EC_ILLEGAL_ARGUMENT, argv[i]);
			}
			UserFirmwareFile = argv[++i];
			if(s[1] == 'u')
			{	firmware_mode = UP_MODE;
			}
			continue;
		}
		if(!STRING_CMP_PARTIAL(s, "-b", 2))		/* base address */
		{	char * endp;
			if(strlen(s) > 2)
			{	base_addr = (int16) strtol(s + 2, &endp, 0);
			}
			else
			{	base_addr = (int16) strtol(argv[++i], &endp, 0);
			}
			continue;
		}
		if(!STRING_CMP_PARTIAL(s, "-a", 2))		/* number of axes */
		{	char * endp;
			user_axes = (int16) strtol(argv[++i], &endp, 10);
			continue;
		}
		if((s[0] == '-') && isdigit(s[1]))
		{	initial_axes = atoi(s + 1);
			continue;
		}
		Error(EC_ILLEGAL_ARGUMENT, argv[i]);
	}
	return 0;
}


int16 Initialize(void)
{	
	if (base_addr)
	{	return dsp_init(base_addr);
	}
	else
	{	return do_dsp();
	}	
}


#define	BOOT_PAGE		(DSP_PAGE_SIZE * sizeof(DSP_BM))

int16 ShowDifferences(int16 ec, char * supposed, char * was)
{	int16 i, l = 0;
	switch (ec)
	{	case EC_PAGE_0_VERIFY:
			printf("Page 0 verify breakdown:\n");
			l = BOOT_PAGE;
			break;
		case EC_PAGE_2_VERIFY:
			printf("Page 2 verify breakdown:\n");
			l = BOOT_PAGE;
			break;
		default:
			printf("Can't show differences for %d.\n", ec);
			return 1;
	}
	for (i = 0; i < l; i++)
	{	if(supposed[i] != was[i])
		{	printf("0x%4.4X: supposed to be 0x%4.4X, was 0x%4.4X\n",
				i, supposed[i], was[i]);
		}
	}
	return 0;
}


int16 MemoryTest(void)
{	int16 failed = 0, db, ab, dc, ac;

	DSP_OUTB((dspPtr)->reset, dummy);	/* disable the DSP */
	
	/* Make sure the DSP is disabled before accessing memory */
	while (dsp_read_dm(0x100) == PCDSP_SIGNATURE)
	{	dsp_write_dm(0x100, 0x0);
	}
	Error(EC_TESTING_DM);

	/* 1:  data memory, linear data. */
	Error(EC_DM_1);
	{	int16 a, v;
		busted_bits = 0;
		for(a = 0x100; a < 0x3FFF; a++)
		{	dsp_write_dm(a, a);
		}
		for(a = 0x100; a < 0x3FFF; a++)
		{	v = dsp_read_dm(a);
			if(v != a)
			{	busted_bits |= (v ^ a);
				Error(EC_DM_1_FAILURE, a, v, a);
				failed = 1;
			}
			else
			{	partial_failure = 1;
			}
		}
		ab = busted_bits;
		if(ab)
		{	Error(EC_DM_1_FAILED);
		}
	}

	/* 2: data memory, random values. */
	Error(EC_DM_2);
	{	int16 a, r, w;
		busted_bits = 0;
		srand(0x641C);
		for(a = 0x100; a < 0x3FFF; a++)
		{	w = rand() % 0x100;
			w <<= 8;
			w |= (rand() % 0x100);
			dsp_write_dm(a, w);
		}
		srand(0x641C);
		for(a = 0x100; a < 0x3FFF; a++)
		{	r = dsp_read_dm(a);
			w = rand() % 0x100;
			w <<= 8;
			w |= (rand() % 0x100);
			if(r != w)
			{	busted_bits |= (r ^ w);
				Error(EC_DM_2_FAILURE, a, r, w);
				failed = 1;
			}
			else
				partial_failure = 1;
		}

		db = busted_bits;
		if(db)
			Error(EC_DM_2_FAILED);
	}

	/* 3: linear data into boot memory. */
	Error(EC_DM_3);
	{	int16 a, w, d;
		char c, cc[32];
      int16   i;
		busted_bits = 0;
		for(a = 0x0; a < BOOT_PAGE; a+=32)
		{	for(i = 0; i < 32; i++)
         	cc[i] = ((a + i) & 0xFF);
		   dsp_write_bm(dspPtr, a, 32, cc);
		}

		for(a = 0x0; a < BOOT_PAGE; a++)
		{	dsp_read_bm(dspPtr, a, 1, &c);
			w = a & 0xFF;
			d = ((int16) c) & 0xFF;
			if(w != d)
			{	busted_bits |= (w ^ d);
				Error(EC_DM_3_FAILURE, a, d, w);
				failed = 1;
			}
			else
				partial_failure = 1;
		}
		ac = busted_bits;
		if(ac)
			Error(EC_DM_3_FAILED);
	}


	/* 4: random data into boot memory. */
	Error(EC_DM_4);
	{	int16 a, r, w;
		char c, cc[32];
      int16   i;
		busted_bits = 0;
		srand(0x641C);
		for(a = 0x0; a < BOOT_PAGE; a+=32)
		{	for(i = 0; i < 32; i++)
   		{	w = rand() & 0xFF;
	   		cc[i] = (char)w;
         }
		   dsp_write_bm(dspPtr, a, 32, cc);
		}
		srand(0x641C);
		for(a = 0; a < BOOT_PAGE; a++)
		{	dsp_read_bm(dspPtr, a, 1, &c);
			w = rand() & 0xFF;
			r = ((int16) c) & 0xFF;
			if(r != w)
			{	busted_bits |= (r ^ w);
				Error(EC_DM_4_FAILURE, a, r, w);
				failed = 1;
			}
			else
				partial_failure = 1;
		}

		dc = busted_bits;
		if(dc)
			Error(EC_DM_4_FAILED);
	}

	/* 5: nvsave of random data into boot memory. */

	if(failed)
	{	if((db == 0xFF) &&
			(ab == 0xFF) &&
			(ac == 0xFF) &&
			(dc == 0xFF))
		{	/* complete failure. */
			Error(EC_PATHETIC);
			return failed;
		}

		if(db != ab)
			Error(EC_DM_ADDRESS, ab);
		else
			Error(EC_DM_DATA, ab);
		if(ac != ab)
			Error(EC_DM_ONLY);
		if((dc != ac) && (db == ab))
			Error(EC_DM_BOOT_ADDRESS, ac);
	}

	return failed;
}


int16 DownloadFirmware(char * filename)
{	char * Buffer;
#ifdef O_VERIFY_BOOT
	char * VerifyBuffer;
	int16 boot_memory_failure = 0;
# endif
	FILE * file;

	file = fopen(filename, "rb");
	if(!file)
		Error(EC_CANT_OPEN, filename);

	Buffer = (char*) calloc(1, 4 * BOOT_PAGE);
	if(!Buffer)
		Error(EC_OUT_OF_MEMORY);
#ifdef O_VERIFY_BOOT
	VerifyBuffer = (char*) calloc(1, 4 * BOOT_PAGE);
	if(!VerifyBuffer)
		Error(EC_OUT_OF_MEMORY);
# endif

	fread(Buffer, 1, 4 * BOOT_PAGE, file);
	DSP_OUTB((dspPtr)->reset, 0xFF);	/* initiate a reset */
	dsp_write_bm(dspPtr, 0, BOOT_PAGE, Buffer);
	dsp_write_bm(dspPtr, 2 * BOOT_PAGE, BOOT_PAGE, &(Buffer[2 * BOOT_PAGE]));

#ifdef O_VERIFY_BOOT
	dsp_read_bm(dspPtr, 0, BOOT_PAGE, VerifyBuffer);
	dsp_read_bm(dspPtr, 2 * BOOT_PAGE, BOOT_PAGE, &(VerifyBuffer[2 * BOOT_PAGE]));

	if(memcmp(VerifyBuffer, Buffer, BOOT_PAGE))
	{	Error(EC_PAGE_0_VERIFY);
		boot_memory_failure++;
		if(verbose)
		{	ShowDifferences(EC_PAGE_0_VERIFY, VerifyBuffer, Buffer);
		}
	}
	if(memcmp(&(VerifyBuffer[2 * BOOT_PAGE]),
				&(Buffer[2 * BOOT_PAGE]),
				BOOT_PAGE))
	{	Error(EC_PAGE_2_VERIFY);
		boot_memory_failure++;
		if(verbose)
		{	ShowDifferences(EC_PAGE_2_VERIFY, VerifyBuffer, Buffer);
		}
	}

	free(VerifyBuffer);
# endif

	free(Buffer);
	fclose(file);
	Error(EC_FIRMWARE_DOWNLOADED, filename);
	Error(dsp_rset());

	return 0;
}


int16 Reconfigure(int16 axis)
{	controller_idle(axis);
	dsp_set_stepper(axis, TRUE);
	dsp_set_step_speed(axis, FAST_STEP_OUTPUT);
	dsp_set_closed_loop(axis, FALSE);		/* count the step output. */
	set_home(axis, NO_EVENT);				/* disable input sensors. */
	set_amp_fault(axis, NO_EVENT);
	set_positive_limit(axis, NO_EVENT);
	set_negative_limit(axis, NO_EVENT);
	return 0;
}


int16 dsp_delay(int16 ms)
{	int16 start, sample_rate, duration;

	sample_rate = dsp_sample_rate();
	duration = (int16)((ms * 1000L) / sample_rate);
	
	start = dsp_read_dm(0x11F);		/* read DSP's sample clock */
	while(((int16)(dsp_read_dm(0x11F) - start)) < duration)
		;
	
	return 0;
}


int16 Moving(int16 axis)
{	int16 i, first, current, old, moving = 0; 

	dsp_delay(40);		/* let the DAC and VFC settle */
	current = first = dsp_encoder(axis);
	
	for (i = 0; i < 25; i++)
	{	old = current;
		dsp_delay(4);	
		current = dsp_encoder(axis);
		if ((current > old) || (current > first))
		{	moving++;
		}
		if (current < old)
		{	return FALSE;
		}
	}
	return moving;
}

int16 NewOffset[PCDSP_MAX_AXES];

int16 SetOffset(int16 axis, int16 offset)
{	Error(set_dac_output(axis, offset));
	NewOffset[axis] = offset;
	return 0;
}

int16 FindThreshold(int16 axis)
{	int16 i, current = 0, old, range;
	
	SetOffset(axis, current);	 	
	range = OFFSET_RANGE;
	
	for (i = 0; i < 11; i++)	
	{ 	old = current;
		range >>= 1;
		
		if(Moving(axis))
		{	current = (old - range);
		}
		else
		{	current = (old + range);
		}
		SetOffset(axis, current);	 	
	}		
	return current;
}


int16 AdjustOffset (int16 axis)
{	int16 offset, wait, good=FALSE, argh;

	printf("%1d ", axis);

	/*	First, verify that there is hardware for the axis. */
	SetOffset(axis, OFFSET_RANGE);
	if(!Moving(axis))
	{ 	Error(EC_AXIS_PATHETIC, axis);
		return !good;
	}
	offset = FindThreshold(axis);
	
	/*	Now, verify that the threshold offset is valid. */
	while(!Moving(axis) && (offset < OFFSET_RANGE))
	{	SetOffset(axis, offset += 1);
		printf("+");
		fflush(stdout);
	}
	if(offset >= OFFSET_RANGE)
	{	printf("\n");
		return Error(EC_TOO_MUCH_OFFSET, axis, offset);
	}

	/* Next, decrease the offset until we stop moving. */
	for(argh = 0; !good && argh < 3; argh++)
	{	while (Moving(axis) && (offset > (-OFFSET_RANGE)))
		{	SetOffset(axis, offset -= 1);
			printf("-");
			fflush(stdout);
		}

		if(offset <= (-OFFSET_RANGE))
		{	printf("\n");
			return Error(EC_TOO_MUCH_OFFSET, axis, offset);
		}

		SetOffset(axis, offset -= 10);	/* set a deadband offset */
		printf("-: %d\n", offset);
		good = TRUE;
		for(wait = 0; good && (wait < 25); wait++)
		{	good = !Moving(axis);
			printf(":");
			fflush(stdout);
		}
		printf("\n");
	}
	if(!good)
	{	Error(EC_AXIS_PATHETIC, axis);
	}

	return !good;
}

int16 WriteOffset(int16 axis)
{	DSP_DM offset = (DSP_DM) NewOffset[axis];
 	pcdsp_set_boot_config_struct(dspPtr, axis, CL_INTERNAL_OFFSET, NULL, &offset);
	return 0;
}

int16 display_position(int16 axis)
{	double p;
	int16 r = get_position(axis, &p);
	if(!r)
	{	printf("%12.0f\b\b\b\b\b\b\b\b\b\b\b\b", p);
		fflush(stdout);
	}
	return r;
}


int16 MotionTest(int16 axis)
{	time_t max_t;
	int16 m;
	double p = 0, q;

	Error(EC_MOTION_TEST, axis);
	dsp_set_step_speed(axis, SLOW_STEP_OUTPUT);
	while(controller_run(axis))
		;
	set_position(axis, 0.0);

	for(m = 0; m < Points; m++)
	{	q = p - MotionFinal[m];
		if(q < 0)
			q = -q;
		p = MotionFinal[m];
		max_t = (time_t)(q/MotionVelocity);
		max_t += time(NULL) + CUSHION;
		while(!motion_done(axis))
			;
		start_move(axis, MotionFinal[m], MotionVelocity, MotionAccel);
		while(!axis_done(axis) && (time(NULL) < max_t))
		{	display_position(axis);
		}
		if(time(NULL) >= max_t)
		{	return Error(EC_MOTION_FAILED, axis);
		}
		if(axis_state(axis) > 2)
		{	return Error(EC_MOTION_FAILED, axis);
		}
	}

	return Error(EC_MOTION_OK, axis);
}


void disable_axes(int16 last_good)
{	int16 axis;
	
	for(axis = last_good; axis < PCDSP_MAX_AXES; axis++)
	{	set_boot_axis(axis, FALSE);
	}
}	


int16 read_board_type(void)
{	int16 board_type, i, temp;

	board_type = (dsp_read_dm(0x78) & 0xF);
	for(i = 0; i < 0xF; i++)
	{	dsp_write_dm(0x78, i);
		temp = dsp_read_dm(0x78) & 0xF;
		if(board_type != temp)
		{	board_type = -1;	/* older board, identification not supported */	
			break;
		}
	}
	return board_type;
}


#ifdef MEI_VW
int MAIN(int argc, char * argv[])
#else
int main(int argc, char * argv[])
#endif
{	int16 axis, last_good = 0, good;

	Arguments(argc, argv);
	Error(EC_CONFIG_VERSION);
	Initialize();

	switch(firmware_mode)
	{	
		case CONFIG_MODE:
		{	int16 board_type; 
			if(MemoryTest())
			{	return 1;
			}
			board_type = read_board_type();
			if((board_type == SERCOS_PC) || (board_type == SERCOS_STD) || (board_type == SERCOS_104))
			{	FirmwareFile = "8AXISSER.ABS";
				DownloadFirmware(FirmwareFile);
				last_good = PCDSP_MAX_AXES;
			}
			else
			{	DownloadFirmware(FirmwareFile);
				if (warning)
					Error(EC_WARNING_MSG);		/* Warning before writing to DACs */
				for(axis = 0; axis < dsp_axes(); axis++)
				{	Reconfigure(axis);
					good = AdjustOffset(axis) == 0;
					if(good)
					{	good = MotionTest(axis) == 0;
					}
					if(good && last_good == axis)
					{	last_good = axis + 1;
					}
				}

				printf("This is a%s %d axis controller.\n", (last_good == 8? "n" : ""), last_good);
				if((last_good < 1) || (last_good > PCDSP_MAX_AXES))
				{	printf("Which is impossible!\n");
					return 1;
				}
				DownloadFirmware(UserFirmwareFile);
			}
			Error(EC_CONFIG_VERSION);
			Error(EC_FIRMWARE_VERSION);
			if(user_axes != 0)
			{	last_good = user_axes;
			}
			disable_axes(last_good);
			mei_checksum();	
			Error(EC_RESET, dsp_rset());
			dspPtr->axes = pcdsp_axes(dspPtr);
			printf("This controller is configured for %d ax%ss.\n\n",
				dsp_axes(), ((dsp_axes() > 1)?"e":"i"));

			if(dsp_axes() < last_good)
			{	last_good = dsp_axes();
			}

			for(axis = 0; axis < last_good; axis++)
			{	WriteOffset(axis);
			}
			printf("Reconfigured.  Checksum=0x%4.4X.\n", mei_checksum());
			Error(dsp_rset());	/* activate our boot memory changes! */
		}	break;

		case DOWN_MODE:
		{	DownloadFirmware(UserFirmwareFile);
			pcdsp_save(dspPtr);
			Error(dsp_rset());	/* activate our boot memory changes! */
		}	break;
		
		case UP_MODE:
		{	Error(upload_firmware_file(UserFirmwareFile));
			Error(EC_FIRMWARE_UPLOADED, UserFirmwareFile);
		}	break;
	}
	return 0;
}
