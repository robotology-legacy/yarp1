/* 
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
*/

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


#include "bttv.h"
#define BTTV_MAX 6

static int bttv_num;
static struct bttv bttvs[BTTV_MAX];

int irq_debug = 1;

int count=0;
int flag = 1;

volatile uint8_t	 *regbase8;
volatile uint32_t    *regbase;    /* device has 32-bit registers */

/* Funcs */

static void bt848_cap(struct bttv * btv,int state);

/********************************************************************************
	Bright
*********************************************************************************/
int 
get_bt848_bright(struct bttv *btv)
{
	int bright;
	bright=btread(BT848_BRIGHT);
	return bright;
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
	Find the bt848.
	Does it exist in the machine?
*********************************************************************************/
int 
attach_bt848(int device_id)
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

    printf("bttvx_reg: IRQ 0x%x\n", info.Irq);

	/* Try to map registers */
	btv->vbip = VBIBUF_SIZE;
	btv->bt848_adr = info.CpuBaseAddress[0];
	btv->irq = info.Irq;

    regbase = (uint32_t *) mmap_device_memory(NULL, 
								 info.BaseAddressSize[0],
								 PROT_READ|PROT_WRITE|PROT_NOCACHE, 
								 MAP_TYPE,
								 PCI_MEM_ADDR(info.CpuBaseAddress[0]));
								 //reg_adress);

	regbase8 = (uint8_t *) mmap_device_memory(NULL, 
								 info.BaseAddressSize[0],
								 PROT_READ|PROT_WRITE|PROT_NOCACHE, 
								 MAP_TYPE,
								 PCI_MEM_ADDR(info.CpuBaseAddress[0]));
								 //reg_adress);

	
	//did the map work?
	if(regbase == MAP_FAILED)
		printf("bttvx: The map failed: %s\n", strerror(errno));
	else
		printf("bttvx: The map Suceeded: %s\n", strerror(errno));

	btv->bt848_mem = (unchar *) regbase8;

}

/*************************************************************************
 *	Main function. Activate the resorce manager	
 *
 *************************************************************************/

int main(int argc,char * argv[]) //Video format, device id, width and height
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


	//Check the arguments
	
	if ( argc == 1 || argc > 3)
	{
		printf("Use: ./bttvx_reg [device_id]\n");
		exit(0);
	}
	

	//Check driver index
	if (argv[1] != NULL)
	{
		device_id = atoi(argv[1]);
	}
	else	//If not specified check for other cards
	{
		device_id = 0;
	}

	printf("bttvx_reg: Looking registers in dev %d\n",device_id);
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

	printf("BT848_DSTATUS				0x%x \n",btread(BT848_DSTATUS)); 
	printf("BT848_IFORM					0x%x \n",btread(BT848_IFORM)); 
	printf("BT848_TDEC					0x%x \n",btread(BT848_TDEC)); 
	printf("BT848_E_CROP				0x%x \n",btread(BT848_E_CROP)); 
	printf("BT848_O_CROP				0x%x \n",btread(BT848_O_CROP)); 
	printf("BT848_E_VDELAY_LO			0x%x \n",btread(BT848_E_VDELAY_LO)); 
	printf("BT848_O_VDELAY_LO			0x%x \n",btread(BT848_O_VDELAY_LO)); 
	printf("BT848_E_VACTIVE_LO			0x%x \n",btread(BT848_E_VACTIVE_LO));
	printf("BT848_O_VACTIVE_LO			0x%x \n",btread(BT848_O_VACTIVE_LO));
	printf("BT848_E_HDELAY_LO			0x%x \n",btread(BT848_E_HDELAY_LO));
	printf("BT848_O_HDELAY_LO			0x%x \n",btread(BT848_O_HDELAY_LO));
	printf("BT848_E_HACTIVE_LO			0x%x \n",btread(BT848_E_HACTIVE_LO));
	printf("BT848_O_HACTIVE_LO			0x%x \n",btread(BT848_O_HACTIVE_LO));
	printf("BT848_E_HSCALE_HI			0x%x \n",btread(BT848_E_HSCALE_HI));
	printf("BT848_O_HSCALE_HI			0x%x \n",btread(BT848_O_HSCALE_HI));
	printf("BT848_E_HSCALE_LO			0x%x \n",btread(BT848_E_HSCALE_LO));
	printf("BT848_O_HSCALE_LO			0x%x \n",btread(BT848_O_HSCALE_LO));
	printf("BT848_BRIGHT				0x%x \n",btread(BT848_BRIGHT));
	printf("BT848_E_CONTROL				0x%x \n",btread(BT848_E_CONTROL));
	printf("BT848_O_CONTROL				0x%x \n",btread(BT848_O_CONTROL));
	printf("BT848_CONTRAST_LO			0x%x \n",btread(BT848_CONTRAST_LO));
	printf("BT848_SAT_U_LO				0x%x \n",btread(BT848_SAT_U_LO));
	printf("BT848_SAT_V_LO				0x%x \n",btread(BT848_SAT_V_LO));
	printf("BT848_HUE 					0x%x \n",btread(BT848_HUE ));
	printf("BT848_E_SCLOOP				0x%x \n",btread(BT848_E_SCLOOP));
	printf("BT848_O_SCLOOP				0x%x \n",btread(BT848_O_SCLOOP));
	printf("BT848_OFORM					0x%x \n",btread(BT848_OFORM));
	printf("BT848_E_VSCALE_HI			0x%x \n",btread(BT848_E_VSCALE_HI));
	printf("BT848_O_VSCALE_HI			0x%x \n",btread(BT848_O_VSCALE_HI));
	printf("BT848_E_VSCALE_LO 			0x%x \n",btread(BT848_E_VSCALE_LO));
	printf("BT848_O_VSCALE_LO			0x%x \n",btread(BT848_O_VSCALE_LO));
	printf("BT848_TEST					0x%x \n",btread(BT848_TEST));
	printf("BT848_ADELAY				0x%x \n",btread(BT848_ADELAY));
	printf("BT848_BDELAY				0x%x \n",btread(BT848_BDELAY));
	printf("BT848_ADC					0x%x \n",btread(BT848_ADC));
	printf("BT848_E_VTC					0x%x \n",btread(BT848_E_VTC));
	printf("BT848_O_VTC					0x%x \n",btread(BT848_O_VTC));
	printf("BT848_SRESET				0x%x \n",btread(BT848_SRESET));
	printf("BT848_TGLB 					0x%x \n",btread(BT848_TGLB ));
	printf("BT848_TGCTRL				0x%x \n",btread(BT848_TGCTRL));
	printf("BT848_COLOR_FMT 			0x%x \n",btread(BT848_COLOR_FMT));
	printf("BT848_COLOR_CTL				0x%x \n",btread(BT848_COLOR_CTL));
	printf("BT848_CAP_CTL				0x%x \n",btread(BT848_CAP_CTL));
	printf("BT848_VBI_PACK_SIZE			0x%x \n",btread(BT848_VBI_PACK_SIZE));
	printf("BT848_VBI_PACK_DEL			0x%x \n",btread(BT848_VBI_PACK_DEL));
	printf("BT848_FCAP					0x%x \n",btread(BT848_FCAP));
	printf("BT848_PLL_F_LO				0x%x \n",btread(BT848_PLL_F_LO));
	printf("BT848_PLL_F_HI				0x%x \n",btread(BT848_PLL_F_HI));
	printf("BT848_PLL_XCI				0x%x \n",btread(BT848_PLL_XCI));
	printf("BT848_DVSIF					0x%x \n",btread(0x0FC));
	printf("BT848_INT_STAT				0x%x \n",btread32(BT848_INT_STAT));
	printf("BT848_INT_MASK				0x%x \n",btread32(BT848_INT_MASK));
	printf("BT848_GPIO_DMA_CTL(16bits)	0x%x \n",btread32(BT848_GPIO_DMA_CTL)); 
	printf("BT848_I2C					0x%x \n",btread32(BT848_I2C)); 
	printf("BT848_GPIO_OUT_EN(24bits)	0x%x \n",btread32(BT848_GPIO_OUT_EN)); 
	printf("BT848_GPIO_DATA(24 bits)	0x%x \n",btread32(BT848_GPIO_DATA)); 

}

#ifdef  __cplusplus
}
#endif

