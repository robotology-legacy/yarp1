/* This is a QNX6 application for PCI bus communications to a 
   Galil motion controller */

#include <process.h>  // declares getpid()
#include <unistd.h>  // declares delay()

#include <inttypes.h>

#include <sys/syspage.h>

#include <dmcqnx.h>

#include <YARPString.h>

#include <Qnx6PCIStartup.h>
extern CQnx6PCIStartup g_objPCIStartup;


void PrintError(long rc);

int main(int argc, char* argv[])
{
   int            i;
   long           rc = 0;            /* Return code */
   int            fInMotion = FALSE; /* Motion complete flag */
   char           buffer[256] = "";  /* Response from controller */

   unsigned char			  command[256]= "";

   ULONG			  command_size = 0;
   pid_t          proxy;
   USHORT         usStatus;
   HANDLEDMC      hdmc = -1;         /* Handle to controller */
   CONTROLLERINFO controllerinfo;    /* Controller information structure */

   YARPString c;

   bool quit = false;

   uint64_t cps, cycle1, cycle2, ncycles;

   float sec;
	
   argc = argc;
   argv = argv;

   printf("Simple pci bus communications example for Galil Motion Controllers\n\n");

   memset(&controllerinfo, 0, sizeof(controllerinfo));

   controllerinfo.cbSize = sizeof(controllerinfo);
   controllerinfo.usModelID = MODEL_1800;
   controllerinfo.fControllerType = ControllerTypePCIBus;
   controllerinfo.ulTimeout = 20000;
   //controllerinfo.ulTimeout = 10;

   controllerinfo.ulDelay = 20;
   /* If you have more than 1 Galil PCI bus controller, use the serial
      number to identify the controller you wish to connect to */
   controllerinfo.ulSerialNumber = 0; 
   controllerinfo.pid = getpid();
 
   DMCInitLibrary();

   //DMCDiagnosticsOn(NULL,  "//root//asd//GalilNTO//trace.txt", 0);

   // Check to see if we correctly initialized access to the PCI server
	if (g_objPCIStartup.GetErrCode())
	{
		printf("Qnx6 PCI initialization failed.  Error message returned:  %s\n", g_objPCIStartup.GetErrMsg());      	
		return 0L;
	}

   /* Open the connection */   
   rc = DMCOpen(&controllerinfo, &hdmc);
   if (rc)
   {

	  fprintf(stdout,"Ecco\n");
      PrintError(rc);
      return rc;  
   }





	while(cin>> c)

	{

		

		if (c == "exit")

		{

			break;

		}

		else 

		{

			//memcpy(command,c.c_str(),c.length());

			//command[c.length() + 1] = ';';

			c += ";";

			/*

			rc = DMCCommand_AsciiToBinary(hdmc, 

										 (char *) c.c_str(), 

										  c.length(), 

										  command,

										  255, 

										  &command_size);

			*/

			cycle1=ClockCycles( );

			rc = DMCCommand(hdmc,(char *) c.c_str(), buffer, sizeof(buffer));

			/*

			rc = DMCBinaryCommand(hdmc,

								 command, 

								 command_size,

								 buffer, 

								 sizeof(buffer));

			*/

			cycle2=ClockCycles( );

			ncycles=cycle2-cycle1;



			cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;

			sec=(float)ncycles/cps;



			printf("The command timing is %f \n",sec);

			printf("rc is -> %d\n",rc);



			printf("%s\n",c.c_str());

		    if (rc)

				PrintError(rc);

			else

			{

				printf("%s\n", buffer);      

			}     

		}

	

	};	

   rc = DMCClose(hdmc);
   if (rc)
   {
      PrintError(rc);
      return rc;  
   }

   printf("\nDone\n");
   
   return 0L;
}

void PrintError(long rc)
{
   printf("An error has occurred. Return code = %ld\n", rc);
   fflush(stdout);
}

