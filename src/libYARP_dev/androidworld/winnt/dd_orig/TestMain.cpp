/*  written by John Bergeron   for Android World Inc.
 *
 *  This is linux program for high speed access of the Touch Sensor Board 
 *  
 */


// to compile for linux, leave the next line defined / uncommented.                  
//#define LINUX  1


// compile for linux: (case sensitive)
// gcc -O2 -o analog4 analog4.c
// then run by:   ./analog4


/*
be careful making changes especially to low level functions.
in particular, optimizing while compiling may ruin the delay thats
critical to clk timing (see notes below about 50%) especially high speed.
Also watch clk speed recommend below 400 KHZ.
*/



/*
old interface notes, disregard
CHIP/PIN    PAR.PORT.PIN var     J5
clk/16      1  c0               1
dout/14     15  e1              4
din/17      14  c1              2

chip select, inA  pin2 d0        3
B  port.pin3    d1               5  

board select inA  pin4  d2      7
B  pin5  d3                     9
enable    pin8  d6
*/



#if LINUX 
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <asm/io.h> 
#include <unistd.h>

#else
// dos version
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <dos.h>
#include <string.h>
#include <Windows.h>
// #include <math.h>

#endif


/* globals */

// this test program is using
// a separate global variable assigned to
// each digital input and output on the printer port,
// accessed via the parOut() function.
// this may be changed to standard bit shifting/addressing for
// the standard i/o card

int chip_sel_A, chip_sel_B, board_sel_A, board_sel_B, board_enable, clk, d_in, d_out;

unsigned short PPORT = 0x378;  // parallel port address

int finger1[8];



/* prototypes */

void parOut(void);       // sends data through printer port
int adc(int channel);    // runs adc chip
void chipsel(int chipNum);  // select 1 of 4 chips on the board (0 - 3)



void
chipsel(int chipNum)
{
   //  chip select, to start the conversion.
   //chip select low to select chip to start conversion.

   switch(chipNum)
   {
      case 0:
      chip_sel_A=chip_sel_B=0;
      break;
  
      case 1:
      chip_sel_A=1; chip_sel_B=0;
      break;
  
      case 2:
      chip_sel_A=0; chip_sel_B=1;
      break;
  
      case 3:
      chip_sel_A=chip_sel_B=1;
      break;
   }  
   parOut();

   board_enable=0;   // this goes through demux chip.  
   parOut(); // send above signal out
}

void
chipunsel(void)
{
   // chip unselect.

   // conversion is done.

   // it is necessary to keep the "chip select" signal high when not
   // using a chip.  each chip has its own unique chip select signal.
   
   
   board_enable =  1; // this disables all chips
   parOut();
}


int 
adc(int channel)
{
   // this function controls the previously selected adc chip
   // to get any one of its 8
   // channels of analog input

   int delay;
   int result=0;

   // mux values to shift into adc chip 
   int ch[8] [4] =
   {
      {1, 0, 0, 0},
      {1, 1, 0, 0}, 
      {1, 0, 0, 1}, 
      {1, 1, 0, 1},
      {1, 0, 1, 0}, 
      {1, 1, 1, 0}, 
      {1, 0, 1, 1},
      {1, 1, 1, 1}
   };
   

   // now the mux data gets clocked in, so the adc chip knows
   // which one of the 8 analog channels
   // to check.

   d_in=1;  // start bit on data line
   parOut();

   // when using on different speed computers, keep in mind that the adc chip
   // clock frequency should be kept between 10 khz to 400 khz

   clk=1;  // clk = high, clock in the data
   parOut();

   clk=0;  // clk = low
   parOut();

   d_in = ch[channel] [0];  // set up data line with first mux
   parOut();

   clk=1;  // clk = high, clock in the data
   parOut();

   clk=0;  // clk = low
   parOut();

   d_in = ch[channel] [1];  
   parOut();

   clk=1;  // clk = high, clock in the data
   parOut();
   parOut();                 // high and low clock, for 50% clock duty cycle   

   clk=0;  // clk = low
   parOut();

   d_in = ch[channel] [2];  
   parOut();

   clk=1;  // clk = high, clock in the data
   parOut();
   parOut();                 // high and low clock, for 50% clock duty cycle   

   clk=0;  // clk = low
   parOut();

   d_in = ch[channel] [3];
   parOut();

   clk=1;  // clk = high, clock in the data
   parOut();
   parOut();                 // high and low clock, for 50% clock duty cycle   

   clk=0;  // clk = low
   parOut();


   // now we will recieve the digital signal from the adc
   // which will be the digital equivalent of the analog signal.
   // (input refers to inputting to computer).

   clk=1;  // clk = high, clock out the result
   parOut();
   parOut();                 

   clk=0;  // clk = low
   parOut();

   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      result += 128;  // msb first

   clk=0;  // clk = low
   parOut();


   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      result += 64;  

   clk=0;  // clk = low
   parOut();


   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      result += 32;  

   clk=0;  // clk = low
   parOut();


   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      result += 16;  

   clk=0;  // clk = low
   parOut();


   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      result += 8;

   clk=0;  // clk = low
   parOut();



   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      result += 4;  

   clk=0;  // clk = low
   parOut();


   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      result += 2;  

   clk=0;  // clk = low
   parOut();



   clk=1;  // clk = high, clock out the result
   parOut();

   parOut();
   if (d_out)
      ++result;   // lsb


   clk=0;  // clk = low
   parOut();

   return result;
}


void parOut() 
{
	int locresult;

	unsigned char cat = 0;

	if (chip_sel_A) cat += 1;
	if (chip_sel_B) cat += 2;
	if (board_sel_A) cat += 4;
	if (board_sel_B) cat += 8;
	if (board_enable) cat += 64;

	_outp( PPORT, cat );
  
	cat=0;

	if (clk == 0)  
		cat += 1;
	if (d_in == 0)  
		cat += 2;
	_outp( PPORT+2, cat );


	locresult=_inp(PPORT + 1);
	d_out=0;
	if(locresult & 8)
		d_out = 1;


// delay to avoid going over 400k clk rate
//usleep(1);  // may be needed for computers over 266 MHZ, adjust delay as necessary

}

void InitTouchBoard()
{
	chip_sel_A   = 0;
	chip_sel_B   = 0;
	board_sel_A  = 0;
	board_sel_B  = 0;
	board_enable = 0;
	clk          = 0;
	d_in         = 0; 
	
   parOut();

   chipunsel();
   clk = 0;
   parOut();

}

unsigned char ReadTouchSensor(unsigned char chip,unsigned char channel)
{
	unsigned char value;
	chipsel(chip);
	value = adc(channel); // get one channel of analog input
	chipunsel();

	return value;

}

void main()
{
	int m,index;
	
		
	printf("\n\n\n\n\n");

	InitTouchBoard();
	/*for (index = 0; index<100000; index++)
	{
		m = ReadTouchSensor(2,1);
		//printf("Ch2=%3d\r",m);
		printf("\b\b\b\b\b\b\b\b\b\b Ch2=%3d",m);
		Sleep(1);
	}
	printf("\n\n\n\n\n");*/
 






















	char ch;
	int j,i,k,l,n,o,p,counter;

	mainMenu:
	puts("  MAIN MENU enter your choice");
	puts("  a- Get all 8 analog inputs from chip 1");                                                              
	puts("  b- Scan chip 1 continuously"); 
	puts("  c- ");
	puts(" ctrl-c to exit ");

	ch=getchar(); 

   switch (ch)
   {
       case 'a':
       case 'A':

		for (j=0; j<4; j++)
		{
		   for (i=0; i<8; i++)
		   {
			  chipsel(j);  // 0 for chip 1, 1 for chip2, etc.  Total of 4 chips on the board. 

			  finger1[i] = adc(i); // get one channel of analog input
			  chipunsel(); // bring chip select high to clear for next conversion.
		   }

		   for (i=0; i<8; i++)
		   {
			  printf("channel %d value= %d\n", i, finger1[i] );
		   }
		}

       break;
     

		case 'b':
		case 'B':
		counter = 0;
		while (1) {
			++ counter;
			if (counter>30000) {
				printf("%d cycles\n", counter);
				Sleep(10);
				goto mainMenu;
			}

			chipsel(2);
			m = adc(3); // get one channel of analog input
			chipunsel();
			Sleep(10);

			printf("Ch3=%d\t", m);

			chipsel(2);
			k = adc(2); // get one channel of analog input
			chipunsel();
			Sleep(10);

			printf("Ch2=%d\t", k);


			chipsel(2);
			l = adc(1); // get one channel of analog input
			chipunsel();
			Sleep(10);

			printf("Ch1=%d\t", l);

			chipsel(2);
			p = adc(0); // get one channel of analog input
			chipunsel();
			Sleep(10);

			printf("Ch0=%d\t", p);

			chipsel(3);
			n = adc(0); // get one channel of analog input
			chipunsel();
			Sleep(10);

			printf("\tCh0 Chip 3=%d\t", n);

			chipsel(1);
			o = adc(0); // get one channel of analog input
			chipunsel();
			Sleep(10);

			printf("   Ch0 Chip 1=%d\n", o);
		}
		break;

      
      default:
         ;
   }   // end of switch
   goto mainMenu;
   
}



