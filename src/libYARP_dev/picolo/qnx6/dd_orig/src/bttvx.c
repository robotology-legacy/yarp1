/****************************************************************************** 
  bttv - Bt848 frame grabber driver

  Copyright (C) 1996,97 Ralph Metzler (rjkm@thp.uni-koeln.de)
  Copyright (C) 2002. QNX version Carlos Beltran (cbeltran@dist.unige.it)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *******************************************************************************/

#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <hw/pci.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <devctl.h>
#include <inttypes.h>
#include <pthread.h>

#include "../include/bttv.h"
#define BTTV_MAX 6
/*
 * Those are the default values for BRIGHT and CONTRAST (for babybot using fgadjuster)
 */
#define BRIGHT_DEFAULT 0
#define HUE_DEFAULT 0
#define CONTRAST_DEFAULT 216
#define SATV_DEFAULT 180
#define SATU_DEFAULT 254 
#define LNOTCH 1
#define LDEC   0
#define CRUSH  0
#define PEAK   0
#define CAGC   1
#define CKILL  0
#define RANGE  1
#define YSLEEP 0
#define CSLEEP 0
#define GAMMA  0
#define DITH_FRAME  0

	/*Globar variables*/
	static int bttv_num;
	static struct bttv bttvs[BTTV_MAX];
	int irq_debug = 1;
	int count=0;
	int flag = 1;
	volatile uint8_t	 *regbase8;
	volatile uint32_t    *regbase;    /* device has 32-bit registers */
	int width=0;
	int height=0;
	int offset_x = 0;
	int offset_y = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
	int m_exit = 0;

	/* Funcs defs */
	//static inline void bt848_bright(uint bright);
	//int get_bt848_bright();
	//static inline void bt848_hue( uint hue);
	//static inline void bt848_contrast( uint cont);
	//static inline void bt848_sat_u( ulong data);
	//static inline void bt848_sat_v( ulong data);
	static void bt848_cap(struct bttv * btv,int state);

	/******************************************************************************** 
	  Tvnorms   
	 *********************************************************************************/   
	struct tvnorm   
	{   
		unsigned long Fsc; //u32   
		unsigned int swidth, sheight; /* scaled standard width, height */   
		unsigned int totalwidth; //u16   
		unsigned char adelay, bdelay, iform; //u8   
		unsigned long scaledtwidth; //u32   
		unsigned int hdelayx1, hactivex1; //16   
		unsigned int vdelay; //u16   
		unsigned char vbipack; //u8   
	};   

	/********************************************************************************   
	  Tvnorms array   
	 *********************************************************************************/   

	static struct tvnorm tvnorms[] = {   
		/* PAL-BDGHI */   
		/* max. active video is actually 922, but 924 is divisible by 4 and 3! */   
		/* actually, max active PAL with HSCALE=0 is 948, NTSC is 768 - nil */   
#ifdef VIDEODAT   
		{ 35468950,   
			924, 576, 1135, 0x7f, 0x72, (BT848_IFORM_PAL_BDGHI|BT848_IFORM_XT1),   
			1135, 186, 924, 0x20, 255
		},   
#else   
		{ 35468950,   
			924, 576, 1135, 0x7f, 0x72, (BT848_IFORM_PAL_BDGHI|BT848_IFORM_XT1),   
			1135, 186, 924, 0x20, 255
		},   
#endif   
		/* NTSC */   
		{ 28636363,   
			768, 480,  910, 0x68, 0x5d, (BT848_IFORM_NTSC|BT848_IFORM_XT0),   
			910, 128, 910, 0x1a, 144
		},   
		/* SECAM L */   
		{ 35468950,   
			924, 576, 1135, 0x7f, 0xb0, (BT848_IFORM_SECAM|BT848_IFORM_XT1),   
			1135, 186, 922, 0x20, 255
		},   
		/* PAL-NC */   
		{ 28636363,   
			640, 576,  910, 0x68, 0x5d, (BT848_IFORM_PAL_NC|BT848_IFORM_XT0),   
			780, 130, 734, 0x1a, 144
		},   
		/* PAL-M */   
		{ 28636363,   
			640, 480, 910, 0x68, 0x5d, (BT848_IFORM_PAL_M|BT848_IFORM_XT0),   
			780, 135, 754, 0x1a, 144
		},   
		/* PAL-N */   
		{ 35468950,   
			768, 576, 1135, 0x7f, 0x72, (BT848_IFORM_PAL_N|BT848_IFORM_XT1),   
			944, 186, 922, 0x20, 144
		},   
		/* NTSC-Japan */   
		{ 28636363,   
			640, 480,  910, 0x68, 0x5d, (BT848_IFORM_NTSC_J|BT848_IFORM_XT0),   
			780, 135, 754, 0x16, 144
		},   
	};   
#define TVNORMS (sizeof(tvnorms)/sizeof(tvnorm)) 

	/********************************************************************************
	  The read function.
	 *********************************************************************************/
	inline int BttvxAcquireBuffer(unsigned char * buf)
	{
		struct bttv * btv = &bttvs[0];

		pthread_mutex_lock(&buf_mutex);
	}	

	inline int	BttvxReleaseBuffer()
	{
		struct bttv * btv = &bttvs[0];

		pthread_mutex_unlock(&buf_mutex);
	}

	int	BttvxSetImageBuffer(int dev,unsigned char * buff)
	{
		struct bttv * btv = &bttvs[0];

		btv->image_buffer = buff;
	}

	int BttvxWaitEvent()
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condvar, &mutex);
	}

	/********************************************************************************
	  Set Bright
	 *********************************************************************************/
	inline void bt848_bright( uint bright)
	{
		btwrite(bright&0xff, BT848_BRIGHT);
	}

	/********************************************************************************
	  Get Bright
	 *********************************************************************************/
	int get_bt848_bright()
	{
		int bright;

		bright=btread(BT848_BRIGHT);
		return bright;
	}

	/********************************************************************************
	  Activa DMA register
	 *********************************************************************************/
	inline static void bt848_dma(struct bttv *btv, uint state)
	{
		if (state)
			btor(3, BT848_GPIO_DMA_CTL);
		else
			btand(~3, BT848_GPIO_DMA_CTL);
	}

	/********************************************************************************
	  Adapting the "virt_to_bus" Linux function into QNX
	 *********************************************************************************/
	u32 virt_to_bus ( u32 * addr )
	{
		off_t offset;
		mem_offset(addr, NOFD, 1, &offset, 0);
		return((u32)offset);
	}

	/********************************************************************************
	  RISC tab.
	 *********************************************************************************/
	static int make_rawrisctab(struct bttv *btv, unsigned int *ro,
							   unsigned int *re, unsigned int *vbuf)
	{
		unsigned long line;
		int flags=btv->cap;
		unsigned long bpl = btv->win.width*btv->win.bpp;
		unsigned long vadr=(unsigned long) vbuf;

		*(ro++)=BT848_RISC_SYNC|BT848_RISC_RESYNC|BT848_FIFO_STATUS_VRE;
		*(ro++)=0;
		*(ro++)=BT848_RISC_SYNC|BT848_FIFO_STATUS_FM1; 
		*(ro++)=0;

		for (line=0; line < btv->win.height; line++)
		{
			*(ro++)=BT848_RISC_WRITE|bpl|BT848_RISC_SOL|BT848_RISC_EOL;
			*(ro++)=virt_to_bus((u32 *) vadr);
			vadr+= (bpl);
		}

		*(ro++) = BT848_RISC_SYNC|BT848_RISC_RESYNC|BT848_FIFO_STATUS_VRO |BT848_RISC_IRQ;
		*(ro++) = 0;
		*(ro++) = BT848_RISC_JUMP;
		*(ro++) = virt_to_bus((u32 *)re);

		btaor(flags, ~0x0f, BT848_CAP_CTL);

		if (flags&0x0f)
			bt848_dma(btv, 3);
		else
			bt848_dma(btv, 0);

		return 0;
	}

	/********************************************************************************
	  Hue
	 *********************************************************************************/
	inline void 
	bt848_hue( uint hue)
	{
		btwrite(hue&0xff, BT848_HUE);
	}

	/********************************************************************************
	  Contrast
	 *********************************************************************************/
	inline void 
	bt848_contrast( uint cont)
	{
		uint conthi;

		conthi=(cont>>6)&4;
		btwrite(cont&0xff, BT848_CONTRAST_LO);
		btaor(conthi, ~4, BT848_E_CONTROL);
		btaor(conthi, ~4, BT848_O_CONTROL);
	}

	/********************************************************************************
	  Sat u
	 *********************************************************************************/
	inline void 
	bt848_sat_u( ulong data)
	{
		ulong datahi;

		datahi=(data>>7)&2;
		btwrite(data&0xff, BT848_SAT_U_LO);
		btaor(datahi, ~2, BT848_E_CONTROL);
		btaor(datahi, ~2, BT848_O_CONTROL);
	}

	/********************************************************************************
	  Sat_v
	 *********************************************************************************/
	inline void 
	bt848_sat_v( ulong data)
	{
		ulong datahi;

		datahi=(data>>8)&1;
		btwrite(data&0xff, BT848_SAT_V_LO);
		btaor(datahi, ~1, BT848_E_CONTROL);
		btaor(datahi, ~1, BT848_O_CONTROL);
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_lnotch 
	// 
	//  Description: This bit is used to include the luma notch filter. For monochrome video, 
	//  the notch filter should not be used. This will output full band-width luminance.
	//  	0 = Enable the Luma notch filter
	//  	1 = Disable the luma notch filter 
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_lnotch(int state)
	{
		if (state) //If state=1 set 0 in LNOTCH (this enables the filter)
		{
			btand(~BT848_CONTROL_LNOTCH, BT848_E_CONTROL);
			btand(~BT848_CONTROL_LNOTCH, BT848_O_CONTROL);
		}
		else //if state = 0 set 1 in the LNOTCH (this disables the filter)
		{
			btor(BT848_CONTROL_LNOTCH, BT848_E_CONTROL);
			btor(BT848_CONTROL_LNOTCH, BT848_O_CONTROL);
		}
	}
	
	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_ldec() 
	// 
	//  Description: The luma decimation filter is used to reduce the high-frequency
	//  component of the luma signal. Useful when scaling to CIF resolution or lower.
	//  	0 = Enable luma decimation using selectable H filter
	//  	1 = Disable luma decimation 
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_ldec(int state)
	{
		if (state) //if state = 1 set a 0 in the LDEC (this enables the filter)
		{
			btand(~BT848_CONTROL_LDEC, BT848_E_CONTROL);
			btand(~BT848_CONTROL_LDEC, BT848_O_CONTROL);
		}
		else //if state = 0 set a 1 in the LDEC (this desables the filter)
		{
			btor(BT848_CONTROL_LDEC, BT848_E_CONTROL);
			btor(BT848_CONTROL_LDEC, BT848_O_CONTROL);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_peak()  
	// 
	//  Description: This bit determines if the normal luma low-pass filters are implemented
	//  via the HFILT bits or if the peaking filters are implemented.
	//  	0 = Normal luma low pass filtering
	//  	1 = Use luma peaking filters 
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_peak(int state)
	{
		if (state)
		{
			btor(BT848_SCLOOP_PEAK, BT848_E_SCLOOP);
			btor(BT848_SCLOOP_PEAK, BT848_O_SCLOOP);
		}
		else
		{
			btand(~BT848_SCLOOP_PEAK, BT848_E_SCLOOP);
			btand(~BT848_SCLOOP_PEAK, BT848_O_SCLOOP);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_cagc()  
	// 
	//  Description: This bit controls the Chroma AGC function. When enabled, Chroma AGC will
	//  compensate for non-standart chroma levels. The compensation is achieved by multiplying 
	//  the incoming chroma signal by value in the range of 0.5 to 2.0
	//  	0 = Chroma AGC Disabled
	//  	1 = Chroma AGC Enabled
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_cagc(int state)
	{
		if (state)
		{
			btor(BT848_SCLOOP_CAGC, BT848_E_SCLOOP);
			btor(BT848_SCLOOP_CAGC, BT848_O_SCLOOP);
		}
		else
		{
			btand(~BT848_SCLOOP_CAGC, BT848_E_SCLOOP);
			btand(~BT848_SCLOOP_CAGC, BT848_O_SCLOOP);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_ckill()  
	// 
	//  Description: This bit determines whether the low color detector and removal circuitry
	//  is enabled
	//  	0 = Low Color Detection and Removal Disabled
	//  	1 = Low Color Detection and Removal Enabled
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_ckill(int state)
	{
		if (state)
		{
			btor(BT848_SCLOOP_CKILL, BT848_E_SCLOOP);
			btor(BT848_SCLOOP_CKILL, BT848_O_SCLOOP);
		}
		else
		{
			btand(~BT848_SCLOOP_CKILL, BT848_E_SCLOOP);
			btand(~BT848_SCLOOP_CKILL, BT848_O_SCLOOP);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_range()  
	// 
	//  Description: Luma Output Range: This bit determines the range for the luminance output
	//  on the Bt879. The range must be limited when using the control codes as video timing.
	//  	0 = Normal operation
	//  	1 = Full-Range output
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_range(int state)
	{
		if (state)
		{
			btor(BT848_OFORM_RANGE, BT848_OFORM);
		}
		else
		{
			btand(~BT848_OFORM_RANGE, BT848_OFORM);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_ysleep()  
	// 
	//  Description: This bit enables putting the luma ADC in sleep mode
	//  	0 = Normal Y ADC operation
	//  	1 = Sleep Y ADC operation
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_ysleep(int state)
	{
		if (state)
		{
			btor(BT848_ADC_Y_SLEEP, BT848_ADC);
		}
		else
		{
			btand(~BT848_ADC_Y_SLEEP, BT848_ADC);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_csleep()  
	// 
	//  Description: This bit enables putting the chroma ADC in sleep mode
	//  	0 = Normal C ADC operation
	//  	1 = Sleep C ADC operation
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_csleep(int state)
	{
		if (state)
		{
			btor(BT848_ADC_C_SLEEP, BT848_ADC);
		}
		else
		{
			btand(~BT848_ADC_C_SLEEP, BT848_ADC);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_crush()  
	// 
	//  Description: When the CRUSH bit is high (adaptive AGC), the gain control mechanism
	//  monitors the A/D's for overflow conditions.If an overflow is detected, the REFOUT
	//  voltage is increased, which increases the input voltage range on the A/D's
	//  	0 = Non-adaptive AGC
	//  	1 = Adaptive AGC 
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_crush(int state)
	{
		if (state)
		{
			btor(BT848_ADC_CRUSH, BT848_ADC);
		}
		else
		{
			btand(~BT848_ADC_CRUSH, BT848_ADC);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_gamma()  
	// 
	//  Description: A value of 0 enables gamma correction removal. The inverse gamma correction
	//  factor of 2.2 or 2.8 is applied and auto-selected by the respective mode NTSC/PAL. A 
	//  value of 1 disables gamma correction removal
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_gamma(int state)
	{
		if (state) // if state = 1 then set a 0 in the GAMMA (this enables gamma correction removal)
		{
			btand(~BT848_COLOR_CTL_GAMMA, BT848_COLOR_CTL);
		}
		else //if state = 0 then set a 1 in the GAMMA (this disables the gamma correction removal)
		{
			btor(BT848_COLOR_CTL_GAMMA, BT848_COLOR_CTL);
		}
	}

	// ===  FUNCTION  ======================================================================
	// 
	//         Name: bt848_set_dithframe()  
	// 
	//  Description: This is a bit of CAP_CTL
	//  	0 = Dither matrix applied to consecutive lines in the field
	//  	1 = Full frame mode
	// 
	//    Author:  Ing. Carlos Beltran
	//  Revision:  none
	// =====================================================================================
	inline void
	bt848_set_dithframe(int state)
	{
		if (state) //Full frame mode 
		{
			btor(BT848_CAP_CTL_DITH_FRAME, BT848_CAP_CTL);
		}
		else //Dither matrix applied 
		{
			btand(~BT848_CAP_CTL_DITH_FRAME, BT848_CAP_CTL);
		}
	}

	/********************************************************************************
	  If Bt848a or Bt849, use PLL for PAL/SECAM and crystal for NTSC

	  Frequency = (F_input / PLL_X) * PLL_I.PLL_F/PLL_C 
	  PLL_X = Reference pre-divider (0=1, 1=2) 
	  PLL_C = Post divider (0=6, 1=4)
	  PLL_I = Integer input 
	  PLL_F = Fractional input 

	  F_input = 28.636363 MHz: 
	  PAL (CLKx2 = 35.46895 MHz): PLL_X = 1, PLL_I = 0x0E, PLL_F = 0xDCF9, PLL_C = 0
	 ********************************************************************************/
	static void 
	set_pll_freq(struct bttv *btv, unsigned int fin, unsigned int fout)
	{
		unsigned char fl, fh, fi;

		/* prevent overflows */
		fin/=4;
		fout/=4;

		fout*=12;
		fi=fout/fin;

		fout=(fout%fin)*256;
		fh=fout/fin;

		fout=(fout%fin)*256;
		fl=fout/fin;

		/*printk("0x%02x 0x%02x 0x%02x\n", fi, fh, fl);*/
		btwrite(fl, BT848_PLL_F_LO);
		btwrite(fh, BT848_PLL_F_HI);
		btwrite(fi|BT848_PLL_X, BT848_PLL_XCI);
	}

	/********************************************************************************
	  Set_pll
	 *********************************************************************************/
	static int set_pll(struct bttv *btv)
	{
		int i;
		unsigned long tv;

		if (!btv->pll.pll_crystal)
			return 0;

		if (btv->pll.pll_ifreq == btv->pll.pll_ofreq) {
			/* no PLL needed */
			if (btv->pll.pll_current == 0) {
				/* printk ("bttv%d: PLL: is off\n",btv->nr); */
				return 0;
			}
			printf ("bttv%d: PLL: switching off\n",btv->nr);
			btwrite(0x00,BT848_TGCTRL);
			btwrite(0x00,BT848_PLL_XCI);
			btv->pll.pll_current = 0;
			return 0;
		}

		if (btv->pll.pll_ofreq == btv->pll.pll_current) {
			/* printk("bttv%d: PLL: no change required\n",btv->nr); */
			return 1;
		}

		printf("bttv%d: PLL: %d => %d ... ",btv->nr,
			   btv->pll.pll_ifreq, btv->pll.pll_ofreq);

		set_pll_freq(btv, btv->pll.pll_ifreq, btv->pll.pll_ofreq);

		for (i=0; i<100; i++) 
		{
			if ((regbase[0]&BT848_DSTATUS_PLOCK))
				regbase[0] = 0;
			else
			{
				btwrite(0x08,BT848_TGCTRL);
				btv->pll.pll_current = btv->pll.pll_ofreq;
				printf("ok\n");
				return 1;
			}
			delay(10);
		}
		btv->pll.pll_current = 0;
		printf("oops\n");
		return -1;
	}

	/********************************************************************************
	  Lets try to put the size
	 *********************************************************************************/
	static void bt848_set_size(struct bttv *btv)
	{
		ushort vscale, hscale;
		ulong xsf, sr;
		ushort hdelay, vdelay;
		ushort hactive, vactive;
		ushort inter;
		unchar crop;
		struct tvnorm *tvn;
		float scalex; 
		float scaley; 
		float scale; 

		float xSize; 
		float ySize; 

		float offsetX;
		float offsetY;

		if (!btv->win.width)
			return;
		if (!btv->win.height)
			return;

		tvn=&tvnorms[btv->win.norm];

		btv->pll.pll_ofreq = tvn->Fsc;
		set_pll(btv);

		inter=(btv->win.interlace&1)^1;

		switch (btv->win.bpp) {
			case 1: 
				btwrite(BT848_COLOR_FMT_Y8, BT848_COLOR_FMT);
				break;
			case 2: 
				btwrite(BT848_COLOR_FMT_RGB16, BT848_COLOR_FMT);
				break;
			case 3: 
				btwrite(BT848_COLOR_FMT_RGB24, BT848_COLOR_FMT);
				break;
			case 4: 
				btwrite(BT848_COLOR_FMT_RGB32, BT848_COLOR_FMT);
				break;
		}

		hactive=btv->win.width;
		if (hactive < 193) {
			btwrite (2, BT848_E_VTC);
			btwrite (2, BT848_O_VTC);
		} else if (hactive < 385) {
			btwrite (1, BT848_E_VTC);
			btwrite (1, BT848_O_VTC);
		} else {
			btwrite (0, BT848_E_VTC);
			btwrite (0, BT848_O_VTC);
		}

		if (btv->win.norm==1) {  /*NTSC*/
			btv->win.cropwidth=640;
			btv->win.cropheight=480;
			btwrite(0x68, BT848_ADELAY);
			btwrite(0x5d, BT848_BDELAY);
			btaor(BT848_IFORM_NTSC, ~7, BT848_IFORM);
			btaor(BT848_IFORM_XT0, ~0x18, BT848_IFORM);
			xsf = (btv->win.width*365625UL)/300000UL;
			hscale = ((910UL*4096UL)/xsf-4096);
			vdelay=btv->win.cropy+0x16;
			hdelay=((hactive*135)/754+btv->win.cropx)&0x3fe;
		} else {				   /*PAL/SECAM*/
			btv->win.cropwidth=768;
			btv->win.cropheight=576;
			//btv->win.cropwidth=W;
			//btv->win.cropheight=H;
			if (btv->win.norm==0) { 
				btwrite(0x7f, BT848_ADELAY);
				btwrite(0x72, BT848_BDELAY);
				btaor(BT848_IFORM_PAL_BDGHI, ~BT848_IFORM_NORM, BT848_IFORM);
			} else {
				btwrite(0x7f, BT848_ADELAY);
				btwrite(0x72, BT848_BDELAY);
				btaor(BT848_IFORM_PAL_BDGHI, ~BT848_IFORM_NORM, BT848_IFORM);
			}

			scalex = 768.0/btv->win.width;
			scaley = 576.0/btv->win.height;
			scale = (scalex < scaley) ? scalex : scaley;
			scalex = scale;
			//scaley = scale / 2.0;
			scaley = scale;
			xSize = 768.0/scalex;
			ySize = 576.0/scaley;

			btaor(BT848_IFORM_XT1, ~0x18, BT848_IFORM);
			//xsf = ((btv->win.width)*36875UL)/30000UL;

			xsf = ((xSize)*36875UL)/30000UL;
			hscale = ((1135UL*4096UL)/xsf-4096);
			/////hscale = 0x29A0; //256
			/////hscale = (((922UL/256UL)-1)*4096UL);

			printf("bttvx: scale: %f\n",scale);
			
			offsetX = (xSize-btv->win.width)/2 + offset_x + 0.5;
			offsetY = (ySize-btv->win.height)/2 + offset_y + 0.5;

			printf("bttvx: offset: %d\n", offsetY);

			vdelay = btv->win.cropy+0x20 + (int)offsetY;
			hdelay = (((int)xSize*186)/922)+(int)offsetX &0x3fe;
		}

		///sr=((btv->win.cropheight>>inter)*512)/btv->win.height-512;
		sr=(((btv->win.cropheight>>inter)/ySize)-1)*512;
		vscale=(0x10000UL-sr)&0x1fff;

		vactive=btv->win.cropheight;

		if (btv->win.interlace) 
		{ 
			btor(BT848_VSCALE_INT, BT848_E_VSCALE_HI);
			btor(BT848_VSCALE_INT, BT848_O_VSCALE_HI);
		} else 
		{
			btand(~BT848_VSCALE_INT, BT848_E_VSCALE_HI);
			btand(~BT848_VSCALE_INT, BT848_O_VSCALE_HI);
		}

		btwrite(hscale>>8, BT848_E_HSCALE_HI);
		btwrite(hscale>>8, BT848_O_HSCALE_HI);
		btwrite(hscale&0xff, BT848_E_HSCALE_LO);
		btwrite(hscale&0xff, BT848_O_HSCALE_LO);

		btwrite((vscale>>8)|(btread(BT848_E_VSCALE_HI)&0xe0), BT848_E_VSCALE_HI);
		btwrite((vscale>>8)|(btread(BT848_O_VSCALE_HI)&0xe0), BT848_O_VSCALE_HI);
		btwrite(vscale&0xff, BT848_E_VSCALE_LO);
		btwrite(vscale&0xff, BT848_O_VSCALE_LO);

		btwrite(hactive&0xff, BT848_E_HACTIVE_LO);
		btwrite(hactive&0xff, BT848_O_HACTIVE_LO);
		btwrite(hdelay&0xff, BT848_E_HDELAY_LO);
		btwrite(hdelay&0xff, BT848_O_HDELAY_LO);

		btwrite(vactive&0xff, BT848_E_VACTIVE_LO);
		btwrite(vactive&0xff, BT848_O_VACTIVE_LO);
		btwrite(vdelay&0xff, BT848_E_VDELAY_LO);
		btwrite(vdelay&0xff, BT848_O_VDELAY_LO);

		crop=((hactive>>8)&0x03)|((hdelay>>6)&0x0c)|
		((vactive>>4)&0x30)|((vdelay>>2)&0xc0);
		btwrite(crop, BT848_E_CROP);
		btwrite(crop, BT848_O_CROP);
	}

	/**************************************************************************
	  Now this is working
	 ***************************************************************************/
	void * InterruptEvent()
	{
		uint32_t stat,astat;
		uint32_t dstat;
		struct bttv * btv = &bttvs[0];
		int res = 0;
		struct sigevent event;
		int error;
		int _waitreturn;

		printf("bttvx: Interrupt thread activated\n");
		ThreadCtl( _NTO_TCTL_IO, 0 );
		SIGEV_INTR_INIT(&event);
		btv->id =InterruptAttachEvent(btv->irq, &event,_NTO_INTR_FLAGS_END | _NTO_INTR_FLAGS_TRK_MSK);

		while (!m_exit)
		{
			_waitreturn = InterruptWait(0,NULL);

			if (_waitreturn == -1) 
				continue;

			/* get/clear interrupt status bits */
			stat = regbase[64];
			astat = stat & regbase[65];
			
			if (!astat) //How the hell where we triggerd, we got no interrupt pending! :)
			{
				InterruptUnmask(btv->irq, btv->id);
				continue;
			}
		
			//Write same data back, clears the pending intrs.
			regbase[64] = stat;

			InterruptUnmask(btv->irq, btv->id);

			/* get device status bits */
			dstat=btread(BT848_DSTATUS);

			//This interrupt called when the RISC program has finished
			if (astat & BT848_INT_RISCI)
			{
				//pthread_mutex_lock(&buf_mutex);
				res = pthread_mutex_trylock(&buf_mutex );
				switch(flag)
				{
					case 1:
						if (res == EOK)
							memcpy(btv->image_buffer,btv->imagebuf_1, width*height*deep);
						flag = 2;
						break;
					case 2:
						if (res == EOK)
							memcpy(btv->image_buffer,btv->imagebuf_2, width*height*deep);
						flag = 3;
						break;
					case 3:
						if (res == EOK)
							memcpy(btv->image_buffer,btv->imagebuf_3, width*height*deep);
						flag = 1;
						break;
				};
				if (res == EOK)
				{
					pthread_mutex_unlock(&buf_mutex);
					pthread_cond_signal(&condvar);
				}
				else
					printf("bttvx:Lost a frame\n");
			}

			if (astat & BT848_INT_VPRES)
			{
				printf("bttvx: Lost conection:%d\n",btv->field_count);
				fflush(stdout);
				//pthread_cond_signal(&condvar);
			}

			if (astat & BT848_INT_VSYNC)
			{
				btv->field_count++;
				//pthread_cond_signal(&condvar);
			}
			if(astat & BT848_INT_FMTCHG) //Change of video input
			{
				btv->win.norm &= (dstat & BT848_DSTATUS_NUML) ? (~1) : (~0); 
				bt848_cap(btv,0);
				bt848_set_size(btv);
				bt848_cap(btv,1);
				printf("bttvx: Changed input format");
			}
			if(astat & BT848_INT_SCERR) //Error, reset capture.
			{
				printf("Error\n");
				fflush(stdout);
				bt848_cap(btv,0);
				bt848_cap(btv,1);
			}
		}
		error = InterruptDetach(btv->id);
		if (error)
			printf("bttvx: error detaching interrupt %d\n",error);

	}

	/********************************************************************************
	  Find the bt848.
	  Does it exist in the machine?
	 *********************************************************************************/
	int attach_bt848(int device_id)
	{
		short  pci_index;
		struct pci_dev_info info;
		void   *hdl;
		int    i;
		struct bttv * btv;
		uint32_t command, latency;
		uint32_t command2;
		uint32_t reg_adress;
		int id;
		struct sigevent event;
		uint16_t hardware_devId;
		pthread_attr_t attr;
		struct sched_param param;

		btv = &bttvs[0];

		btv->bt848_mem=NULL;
		btv->riscmem=NULL;

		memset(&info, 0, sizeof (info));

		btv->phdl = pci_attach( 0 );

		if (btv->phdl < 0) 
		{
			perror("pci_attach");
			exit(EXIT_FAILURE);
		}

		/*
		 * Fill in the Vendor and Device ID for a 3dfx VooDoo3
		 * graphics adapter.
		 */
		info.VendorId = 0x109e;

		info.DeviceId = 0x350;

		btv->hdl = pci_attach_device(0,
									 PCI_INIT_ALL|PCI_SEARCH_VEND|PCI_SEARCH_VENDEV, 
									 device_id, 
									 &info); 

		if (btv->hdl == NULL)
		{
			//Second chance
			info.DeviceId = 0x36e;
			btv->hdl = pci_attach_device(0,
										 PCI_INIT_ALL|PCI_SEARCH_VEND|PCI_SEARCH_VENDEV, 
										 device_id, 
										 &info); 
			if (btv->hdl == NULL)
				return 0;
		}

		if (btv->hdl == 0) 
		{
			perror("pci_attach_device");
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < 6; i++) 
		{
			if (info.BaseAddressSize[i] > 0)
			{
				printf("bttvx: Aperture %d: "
					   "Base 0x%llx Length %d bytes Type %s\n", 
					   i,
					   PCI_IS_MEM(info.CpuBaseAddress[i]) ?
					   PCI_MEM_ADDR(info.CpuBaseAddress[i]) :
					   PCI_IO_ADDR(info.CpuBaseAddress[i]),
					   info.BaseAddressSize[i],
					   PCI_IS_MEM(info.CpuBaseAddress[i]) ?
					   "MEM" : "IO");
			}
		}

		printf("bttvx: IRQ 0x%x\n", info.Irq);

		/* Try to map registers */
		btv->vbip = VBIBUF_SIZE;
		btv->bt848_adr = info.CpuBaseAddress[0];
		btv->irq = info.Irq;

		regbase = (uint32_t *) mmap_device_memory(NULL, 
												  info.BaseAddressSize[0],
												  PROT_READ|PROT_WRITE|PROT_NOCACHE, 
												  MAP_TYPE,
												  PCI_MEM_ADDR(info.CpuBaseAddress[0]));

		regbase8 = (uint8_t *) mmap_device_memory(NULL, 
												  info.BaseAddressSize[0],
												  PROT_READ|PROT_WRITE|PROT_NOCACHE, 
												  MAP_TYPE,
												  PCI_MEM_ADDR(info.CpuBaseAddress[0]));

		//Interrupt mask register = 0 (INT_MASK)
		regbase[65]=0;

		//ThreadCtl( _NTO_TCTL_IO, 0 );

		pthread_attr_init(&attr);
		pthread_attr_getschedparam(&attr, &param);
		param.sched_priority = 63;  
		param.sched_curpriority = 63;     
		pthread_attr_setschedparam(&attr, &param);
		sched_setparam(0, &param); 
		pthread_attr_getschedparam(&attr, &param);
		printf("bttvx: The newly assigned priority is %d.\n", param.sched_priority);

		pthread_create(NULL,&attr,&InterruptEvent,NULL);

		//SIGEV_THREAD_INIT((&event), InterruptEvent,&attr, NULL);

		//btv->id =InterruptAttachEvent(info.Irq, &event, _NTO_INTR_FLAGS_PROCESS);

		if (btv->id==-1)
		{
			fprintf(stderr, "Cannot attach IRQ%d\n", info.Irq);
			return -1;
		}

		//did the map work?
		if(regbase == MAP_FAILED)
			printf("bttvx: The map failed: %s\n", strerror(errno));
		else
			printf("bttvx: The map Suceeded: %s\n", strerror(errno));

		btv->bt848_mem = (unchar *) regbase8;

		//PLL programming. For the moment only PAL.
		btv->pll.pll_crystal = 0;
		btv->pll.pll_ifreq   = 0;
		btv->pll.pll_ofreq   = 0;
		btv->pll.pll_current = 0;

		if (btv->win.norm==1) 
		{ 
			/* 35 MHz crystal installed */
			btv->pll.pll_ifreq=35468950;
			btv->pll.pll_crystal=BT848_IFORM_XT1;
		}
		else
		{
			/* 28 MHz crystal installed */
			btv->pll.pll_ifreq=28636363;
			//btv->pll.pll_ifreq=28618000;
			btv->pll.pll_crystal=BT848_IFORM_XT0;       
		}

		//pci_detach_device(hdl);
	}

	/*************************************************************************
	  Activate capture
	 **************************************************************************/
	static void bt848_cap(struct bttv * btv,int state)
	{
		if (state) 
		{
			btv->cap|=3;
			//bt848_set_risc_jmps(btv);
		}
		else
		{
			btv->cap&=~3;
			//bt848_set_risc_jmps(btv);
		}
	}

	/********************************************************************************
	  Open
	 *********************************************************************************/
	int open_bttvx()
	{
		struct bttv *btv;
		btv = &bttvs[0];

		m_exit = 0;

		btv->vbip = VBIBUF_SIZE;
		//btv->cap |= 0x0c;
		//bt848_set_risc_jmps(btv);

		bt848_cap(btv, 1); //activa capture

		make_rawrisctab(btv, 
						(unsigned int *) btv->risc_1,
						(unsigned int *) btv->risc_2, 
						(unsigned int *) btv->imagebuf_1);
		make_rawrisctab(btv, 
						(unsigned int *) btv->risc_2,
						(unsigned int *) btv->risc_3, 
						(unsigned int *) btv->imagebuf_2);
		make_rawrisctab(btv, 
						(unsigned int *) btv->risc_3,
						(unsigned int *) btv->risc_1, 
						(unsigned int *) btv->imagebuf_3);

		//bt848_cap(btv, 1); //activa capture
		return 1;
	}

	/********************************************************************************
	  Close
	  Close all the things
	 *********************************************************************************/
	int close_bttvx()
	{
		uint32_t stat,astat;
		uint32_t dstat;
		struct bttv *btv;
		int error;
		btv = &bttvs[0];

		if (!m_exit)
			m_exit = 1;
		else
			return 1;

		BttvxAcquireBuffer(NULL);

		InterruptMask(btv->irq, btv->id);

		bt848_cap(btv, 0);
		bt848_dma(btv,0);
		regbase[65]=0;

		stat = regbase[64];
		astat = stat & regbase[65];
		regbase[64] = stat;

		//error = InterruptDetach(btv->id);
		//if (error)
		//	printf("bttvx: error detaching interrupt %d\n",error);

		printf("bttvx: Detaching pci\n");
		pci_detach_device(btv->hdl);
		pci_detach(btv->phdl );

		printf("bttvx: Closing driver\n");
		InterruptUnmask(btv->irq, btv->id);

		BttvxReleaseBuffer();

		return 1;
	}

	/***********************************************************************************
	  Initialization
	 ************************************************************************************/
	static int init_bt848(struct bttv * btv, int video_format)
	{
		int i;

		//Reset the framegrabber
		btwrite(0,BT848_SRESET);

		/* default setup for max. PAL size in a 1024xXXX hicolor framebuffer */

		btv->win.norm = video_format; /* change this to 1 for NTSC */

		btv->win.interlace = 0;
		btv->win.x=0;
		btv->win.y=0;

		switch(btv->win.norm)
		{
			case 0:case 2:
				btv->win.width=width; /* 640 */
				btv->win.height=height; /* 480 */
				btv->win.cropwidth=768; /* 640 */
				btv->win.cropheight=576; /* 480 */
				break;

			case 1:
				btv->win.width = 640; 
				btv->win.height = 480; 
				btv->win.cropwidth = 640; 
				btv->win.cropheight = 480;
				break;
		}

		btv->win.cropx=0;
		btv->win.cropy=0;
		btv->win.bpp=deep;
		btv->win.bpl=btv->win.width*btv->win.bpp;
		btv->win.swidth=1024;
		btv->win.sheight=768;
		btv->cap=0;

		btv->risc_1 = (u32 *) mmap(0,
								   RISCMEM_LEN/2,
								   PROT_READ|PROT_WRITE|PROT_NOCACHE,
								   MAP_PHYS|MAP_ANON,
								   NOFD,
								   0);

		btv->risc_2 = (u32 *) mmap(0,
								   RISCMEM_LEN/2,
								   PROT_READ|PROT_WRITE|PROT_NOCACHE,
								   MAP_PHYS|MAP_ANON,
								   NOFD,
								   0);

		btv->risc_3 = (u32 *) mmap(0,
								   RISCMEM_LEN/2,
								   PROT_READ|PROT_WRITE|PROT_NOCACHE,
								   MAP_PHYS|MAP_ANON,
								   NOFD,
								   0);

		regbase[69] = virt_to_bus( btv->risc_1);

		btv->imagebuf_1 =   (unchar *) mmap(0,
											btv->win.width * btv->win.height * deep,
											PROT_READ|PROT_WRITE|PROT_NOCACHE,
											MAP_PHYS|MAP_ANON | MAP_BELOW16M ,
											NOFD,
											0);
		memset(btv->imagebuf_1, 1,btv->win.width * btv->win.height * deep);

		btv->imagebuf_2 =   (unchar *) mmap(0,
											btv->win.width * btv->win.height * deep,
											PROT_READ|PROT_WRITE|PROT_NOCACHE,
											MAP_PHYS|MAP_ANON | MAP_BELOW16M ,
											NOFD,
											0);
		memset(btv->imagebuf_2, 1,btv->win.width * btv->win.height * deep);

		btv->imagebuf_3 = (unchar *) mmap(0,
										  btv->win.width * btv->win.height * deep,
										  PROT_READ|PROT_WRITE|PROT_NOCACHE,
										  MAP_PHYS|MAP_ANON | MAP_BELOW16M ,
										  NOFD,
										  0);
		memset(btv->imagebuf_3, 1,btv->win.width * btv->win.height * deep);

		//bt848_muxsel(btv, 1);

		//btwrite(0x10, BT848_COLOR_CTL);

		btwrite(0x00, BT848_CAP_CTL);

		//btwrite(0x0ff, BT848_VBI_PACK_SIZE);
		//btwrite(1, BT848_VBI_PACK_DEL);

		btwrite(0xfc, BT848_GPIO_DMA_CTL);

		switch(btv->win.norm)
		{
			case 0:
				btwrite(BT848_IFORM_MUX0 | BT848_IFORM_XTAUTO | BT848_IFORM_PAL_BDGHI,
						BT848_IFORM);
				break;

			case 1:
				btwrite(BT848_IFORM_MUX1 | BT848_IFORM_XTAUTO | BT848_IFORM_NTSC,
						BT848_IFORM);
				break;

			case 2:
				btwrite(BT848_IFORM_MUX2 | BT848_IFORM_XTAUTO | BT848_IFORM_AUTO,
						BT848_IFORM);
				break;
		}

		bt848_set_size(btv);
		
		btwrite(BT848_ADC_RESERVED, BT848_ADC);

		btwrite(BT848_CONTROL_LDEC , BT848_E_CONTROL);
		btwrite(BT848_CONTROL_LDEC , BT848_O_CONTROL);

		if (btv->win.norm == 2) //S-video; Y/C configuration
		{
			btwrite(BT848_CONTROL_COMP | BT848_CONTROL_LDEC, BT848_E_CONTROL);
			btwrite(BT848_CONTROL_COMP | BT848_CONTROL_LDEC, BT848_O_CONTROL);
		}
		
btwrite(0x00, BT848_E_SCLOOP);
		btwrite(0x00, BT848_O_SCLOOP);

		//----------------------------------------------------------------------
		//	Insert default bright and contrast values. Also add adequate filters
		//	This is to aproximate picolo (windows) driver performance  
		//----------------------------------------------------------------------
		bt848_bright(BRIGHT_DEFAULT);
		bt848_hue(HUE_DEFAULT);
		bt848_contrast(CONTRAST_DEFAULT);
		bt848_sat_v(SATV_DEFAULT);
		bt848_sat_u(SATU_DEFAULT);
		bt848_set_lnotch(LNOTCH);
		bt848_set_ldec(LDEC);
		bt848_set_peak(PEAK);
		bt848_set_cagc(CAGC);
		bt848_set_ckill(CKILL);
		bt848_set_range(RANGE);
		bt848_set_ysleep(YSLEEP);
		bt848_set_csleep(CSLEEP);
		bt848_set_crush(CRUSH);
		bt848_set_gamma(GAMMA);
		bt848_set_dithframe(DITH_FRAME);

		//This is the interrupt part, It blocks the QNX system. ??

		regbase[64] = 0xffffffUL;
		regbase[65] = BT848_INT_VSYNC|BT848_INT_FMTCHG|BT848_INT_SCERR|BT848_INT_VPRES|BT848_INT_RISCI;

		return 0;
	}

	/*************************************************************************
	 *	Main function. Activate the resorce manager	
	 *
	 *************************************************************************/
	int init_bttvx(int video_mode, int device_number, int w, int h, int _offset_x, int _offset_y) //Video format, device id, width and height
	{
		int i; 
		/* declare variables we'll be using */
		resmgr_attr_t		resmgr_attr;
		dispatch_t			*dpp;
		dispatch_context_t  *ctp;
		int                 id;
		int video_format = 0;
		int device_id = 0;
		char device_name[100];
		char shell_command[100];
		char buffer[20];
		char buffer2[20];
		int bus, dev_func;

		//Get video format
		video_format = video_mode;

		//Set image offsets
		offset_x = _offset_x;	
		offset_y = _offset_y;

		//Check driver index
		if (device_number != NULL)
		{
			if (device_id < 0 || device_id > BTTV_MAX)
			{
				printf("bttvx: Sorry!, the device id overcomes the maximum number of devices allowed\n");
				exit(0);
			}
			device_id = device_number;
		}
		else	//If not specified check for other cards
		{
			if (pci_attach(0) < 0) 
			{
				perror("pci_attach");
				exit(EXIT_FAILURE);
			}   
			pci_detach(0);
		}

		//Filling passed width and height
		if (w != NULL)
			width = w; //Passed width
		else
			width = W; //Fixed width	
		if (h != NULL)
			height = h; //Passed
		else
			height = H; //Fixed

		printf("bttvx: grabbing with width = %d and height = %d\n",width,height);
		fflush(stdout);

		printf("bttvx: attaching device id : %d\n",device_id);
		fflush(stdout);
		i = attach_bt848(device_id);

		if ( i == 0)
		{
			printf("bttvx: sorry I didn't find the card\n");
			exit(0);
		}

		printf("bttvx: initializing with video format : %d\n", video_format);
		fflush(stdout);

		i = init_bt848(&bttvs[0],video_format); 

	}

#ifdef  __cplusplus
}
#endif

