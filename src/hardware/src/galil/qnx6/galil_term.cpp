/* This is a QNX6 application for PCI bus communications to a 
   Galil motion controller */

#define DMC_DEBUG
#include <process.h>  // declares getpid()
#include <unistd.h>  // declares delay()

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
   char			  command[256]= "";
   pid_t          proxy;
   USHORT         usStatus;
   HANDLEDMC      hdmc = -1;         /* Handle to controller */
   CONTROLLERINFO controllerinfo;    /* Controller information structure */
   YARPString c;
   bool quit = false;
	
   argc = argc;
   argv = argv;

   printf("Simple pci bus communications example for Galil Motion Controllers\n\n");

   memset(&controllerinfo, '\0', sizeof(controllerinfo));

   controllerinfo.cbSize = sizeof(controllerinfo);
   controllerinfo.usModelID = MODEL_1800;
   controllerinfo.fControllerType = ControllerTypePCIBus;
   controllerinfo.ulTimeout = 2000;
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
			rc = DMCCommand(hdmc,(char *) c.c_str(), buffer, sizeof(buffer));
			printf("%s\n",c.c_str());
		    if (rc)
				PrintError(rc);
			else
			{
				printf("%s\n", buffer);      
			}     
		}
	
	};	

   /* Query the state of the inputs */
   /***
   rc = DMCCommand(hdmc, "TI;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);
   else
   {
      int temp;
      temp = atoi(buffer);
      printf("The value of the inputs is %d\n", temp);      
   }     
	**/
  

   /* Wait until after motion */
   /**
   while (!rc && fInMotion)
   {
      rc = DMCCommand(hdmc, "MG_BGX;", buffer, sizeof(buffer));
      if (rc)
         PrintError(rc);
      else
         fInMotion = atoi(buffer);         
   }
	*/
                   
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