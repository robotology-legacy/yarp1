#include <sys/mman.h>

class CQnx6PCIStartup
{

public:
	////////////////////////////////
	CQnx6PCIStartup()
	{
		// Initialize members
		m_nErrCode=0;
		strcpy(m_szErrMsg, "No error.");

		// Get IO privilege for process
    		ThreadCtl(_NTO_TCTL_IO, 0);

		// Try to start the PCI server and attach this process to Galil devices
 
 		int pidx;
 		struct pci_dev_info inf;

		/* Connect to the PCI server */
		pci_attach( 0 );

	    /* Initialize the pci_dev_info structure */
		memset( &inf, 0, sizeof( inf ) );
  		pidx = 0;
  		inf.VendorId = VENDOR_ID;
  		inf.DeviceId = DEVICE_ID;

		// Get access to all Galil PCI Motion Controllers
		void * hDevice=NULL;
		hDevice = pci_attach_device( NULL, PCI_INIT_ALL | PCI_SHARE, pidx, &inf );
		do 
		{
			uintptr_t pusRet;
			for (int i=0; i<6; i++)
			{
				UCHAR uchTest = inf.PciBaseAddress[i] & 0x01;
				if ( uchTest == 0x01 )	// if the LSB is true, this address is IO and not memory-mapped
					pusRet = mmap_device_io( inf.BaseAddressSize[i], inf.PciBaseAddress[i] );
			}
			pidx++;
			hDevice = pci_attach_device( NULL, PCI_INIT_ALL | PCI_SHARE, pidx, &inf );
		}
		while (hDevice) ;		

	}	// end constructor
	////////////////////////////////
	
	////////////////////////////////
	~CQnx6PCIStartup()
	{
		if (!m_nErrCode)
		{
			pci_detach_device( m_pvhDevice );
			pci_detach( m_phPCIServer );
		}
	}
	///////////////////////////////
	
	///////////////////////////////
	int GetErrCode()
	{
		return m_nErrCode;
	}
	
	char * GetErrMsg()
	{
		return m_szErrMsg;
	}
	///////////////////////////////
private:
	short m_nErrCode;
	char m_szErrMsg[256];
	void* m_pvhDevice;
	int m_phPCIServer;
	
};	// end CQnx6PCIStartup 		
