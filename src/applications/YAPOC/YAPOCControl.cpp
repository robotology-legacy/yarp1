#include <sys/kernel.h>
#include <iostream.h>
#include <signal.h>
#include <sys/name.h>

#define DEBUG 0
#include "YAPOCControl.h"
#include "YARPPort.h"

/* pf - added NO_BUFFERS option to ensure messages don't get dropped
 * by the communications library if they are not being read fast
 * enough */
YARPInputPortOf<YAPOCMessage> inCommandPort(YARPInputPort::NO_BUFFERS);
int numPICdaemons; //number of PIC daemons (4 for K4)
K4JointPos pos; //global var to hold 30 motor positions read from encoder

//encoder period
const int encoder_ms = 5;

//encoder thread class
class CEncoderThread : public YARPRateThread
{
public:
  //PIC clients
  CClientPIC p_heads[MAX_PIC_DAEMONS];
protected:
  //MEI clients
  YARPHeadControl m_head;

  // output last position and speed.
  //YARPOutputPortOf <JointSpeed> m_command_channel;        // velocity
  YARPOutputPortOf <K4JointPos>   m_position_channel;       // position
  
  CVisDVector m_position;
  //CVisDVector m_speed;
  CVisDVector m_position_o;
  //CVisDVector m_speed_o;
  
  bool m_initialized;
  void DoInit(void *) 
    {
      //initialize MEIhead
      m_head.Initialize (MEI_DEVICENAME, MEI_INI_FILENAME);
            
      //initialize PIChead
      //PIC processes are named /PIC0, /PIC1, /PIC2, /PIC3
      char procname[50];
      char* index = "0";
      //initialize PIC
      for (int i = 0; i < numPICdaemons; i++){
	printf("PIC Daemon %d : ",i);
	fflush(stdout);
	
	strcpy(procname,PIC_PROCNAME_BASE);
	strcat(procname,index);
	p_heads[i].initialize(procname);
	index[0]++;
	printf("PIC %d pid = %d\n",i,p_heads[i].pid);
	if (p_heads[i].pid == -1)
	  {
	    printf("Could not find %s !!\n",procname);
	  }
	
      }
      m_position.Resize (m_head.GetNJoints());
      m_position_o.Resize (m_head.GetNJoints());
      
      //m_speed.Resize (m_head.GetNJoints());
      //m_speed_o.Resize (m_head.GetNJoints());
      
      m_position = 0; 
      m_position_o = 0;
      
      //m_speed = 0;
      //m_speed_o = 0;
      
      //m_command_channel.Register (COMMANDCHANNELNAME);
      m_position_channel.Register (POSITIONCHANNELNAME);
      
      Lock ();
      m_initialized = true;
      Unlock ();
    }
  
  void DoLoop(void *) 
    {
      //Read position data from MEI motors
      m_head.Input ();
      m_head.GetActualPosition (m_position.data());
      //m_head.GetActualVelocity (m_speed.data());
      
      Lock ();
      m_position_o = m_position;
      //m_speed_o = m_speed;
      Unlock ();
      
      //copy position data read from port into pos
      for (int pp = 0; pp < 8; pp++)
	pos.SetPosition(pp,(float)m_position(pp+1));
      
      //Read position data from PIC motors
      int status,count = 8;
      int encoder_value;
      //loop over each of the 4 groups
      for (int p = 0; p < numPICdaemons; p++){
	for (int m = p_heads[p].nmotors; m >= 1; m--){
	  status = p_heads[p].D_ReadEncoder(m,1,&encoder_value);
	  
	  //status is 0 if okay
	  if (status)
	    printf("Error in reading encoder\n");
	  
	  //copy position to pos
	  pos.SetPosition(count,(float)encoder_value);
	  count++;
	}
     }
      
      //write pos to output port
      m_position_channel.Content() = pos;
      m_position_channel.Write ();
    }
  
  void DoRelease(void *) 
    {
      m_head.Uninitialize ();
    }
  
public:
  CEncoderThread () : YARPRateThread ("encoder_thread", encoder_ms) 
    {
                    m_initialized = false;
    }
  
  virtual ~CEncoderThread () {
  }
  
  inline void GetReading (CVisDVector& p)
    {
      Lock ();
      if (m_initialized)
	{
	 
	  p = m_position_o;
	  //v = m_speed_o;
	}
      else
	{
	  p = 0;
	  //v = 0;
	}
      Unlock ();
    }
};


class YAPOCVirtualControl
{
public:
  //axismap's first row is axistype, 2nd is axis actual number
    int axismap[MAXVIRTUALAXES][2];

  //pics and address_axes keep mapping between PIC actual and physical number.
  int pics_axes[MAXVIRTUALAXES]; //contains the daemon index for each axis
  byte address_axes[MAXVIRTUALAXES]; //contains the actual index for each axis

  //read from config file (see function ReadAxisMap)
  int naxes; //total number of axes
  int nMEIaxes; //number of MEI axes
  int nPICaxes; //number of PIC axes

  //MEID handle
  int meidx;

  //Encoder thread
 CEncoderThread Encoder;

  //Instances of MEI and PIC clients
  YARPHeadControl MEIhead;
  CClientPIC PICheads[MAX_PIC_DAEMONS]; //one of each PIC daemon

 //utility function for reading config files
 void IgnoreComment(FILE *fp);
  void ReadAxisMap(const char * filename);

 //initialization functions
 int Initialize();
 void MapPICDaemons();
 
 //calibration functions
 int CalibrateEach(int vnumber);
  int CalibrateAll();
 
 //position command function
 int PositionCommand(float *positions, short* statusbits, short uselimit_flag);
   /*NOTE for caller of PositionCommand
    *     *   * for MEI axes, all positions must be filled out properly, b/c all will be
    *     *   * sent to the MEID
    *     *   * for PIC axes, status bits have to be set properly to distinguish axes
    *     *   * that have to be moved. only axes to be moved are sent to the PIC daemon
    *     *  */

 //reset encoder function
  int ZeroEncoder (int axis);
  
 //adjust velocities
  int SetVelocity(float *newvel, short *velmode);
};


//global variable
YAPOCVirtualControl virtualcontrol;

//copied from YARPHeadControl's EatLine
void YAPOCVirtualControl::IgnoreComment (FILE *fp)
  {
      // check if line begins with '//'
          for (;;){
	          char c1, c2;
	          fread (&c1, 1, 1, fp);
	          fread (&c2, 1, 1, fp);
	          if (c1 == '/' && c2 == '/'){
		            do{
			                fread (&c1, 1, 1, fp);
			    }
		            while (c1 != '\n' && !feof(fp));
		  }
	          else{
		            fseek (fp, -2, SEEK_CUR);
		            return;         // exit from here.
		  }
	  }
  }
     
    
//Read from file mapping between axes virtual to actual number
//file resides in $YARP_ROOT/conf/axismap.ini
    void YAPOCVirtualControl::ReadAxisMap(const char *filename){
      char type;
      FILE *fp = fopen(filename, "r");
      assert (fp != NULL);
      
      //in case of comments
      IgnoreComment(fp);
      //read number of MEI and PIC axes
      fscanf(fp,"%d %d %d\n", &nMEIaxes, &nPICaxes,&numPICdaemons);
      naxes = nMEIaxes + nPICaxes;
      if (DEBUG) printf("Number of MEI axes = %d, Number of PIC axes = %d, Total = %d, Number of PIC daemons = %d\n",
	     nMEIaxes,nPICaxes,naxes,numPICdaemons);
      //in case of comments
      IgnoreComment(fp);
      //for each axis (virtual number 0 - 30), read how it's mapped to actual number
      //and what motor type it is
      for (int i = 0; i < naxes; i++)
	fscanf(fp,"%d %d\n", &axismap[i][AXISTYPE], &axismap[i][AXISNUMBER]);
      fclose(fp);
      
      for (int j = 0; j < naxes; j++)
	if (DEBUG) printf("%d %d\n",axismap[j][AXISTYPE], axismap[j][AXISNUMBER]);
    }
     
     
 
 void YAPOCVirtualControl::MapPICDaemons(){
   int all_motors = 0;
   
   for (int i = 0; i < numPICdaemons; i++)
     all_motors += PICheads[i].nmotors;
   
 
   int curdaemon = 0;
   int count = PICheads[curdaemon].nmotors;
   for (int a = 0; a < all_motors; a++){
     if (count == 0)
       {
	 curdaemon++;
	 count = PICheads[curdaemon].nmotors;
       }
     
     address_axes[a] = count;
     pics_axes[a] = curdaemon;
     if (DEBUG) printf("addr_axes[%d] = %d pics_axes[%d] = %d\n",a,count,a,curdaemon);
     
     count--;  
     
   }
   
 }
     
    int YAPOCVirtualControl::Initialize()
    {
      if (DEBUG) printf("Initializing virtual control..\n");

      //read mapping
      ReadAxisMap(AXISMAPFILENAME);
      if (DEBUG) printf("Axis map read.\n");
         
      //initialize MEI
      meidx = MEIhead.Initialize(MEI_DEVICENAME, MEI_INI_FILENAME);
      printf("MEI initialized\n");
      
      //enable all MEI axes
      for (int m = 0; m < nMEIaxes; m++){
	MEIhead.EnableAxis (m);
      }
      
      if (meidx == -1){
	printf("Failed to locate MEI daemon\n");
	return -1;
      }
      
      //Reset MEID
      int stat = meid_dsp_reset(meidx);
        if (stat != 0)
	{
	      printf("MEI Reset Failed... Error code %d!\n", stat);
	      return -1;
	}
        else
	  printf("MEI Reset Done...\n");
      

      char procname[50];
      char pic_ini_fname[100];
      
      char* index = "0";
      //initialize PIC
      for (int i = 0; i < numPICdaemons; i++){
	printf("PIC Daemon %d : ",i);
	fflush(stdout);
	
	strcpy(procname,PIC_PROCNAME_BASE);
	strcat(procname,index);

	strcpy(pic_ini_fname,PIC_INI_FILENAME);
	strcat(pic_ini_fname,index);
	
	//this initialize takes care of getting no of motors and reading configuration files
	PICheads[i].initialize(procname,pic_ini_fname);

	if (DEBUG) printf("PIC %d pid = %d\n",i,PICheads[i].pid);
	if (PICheads[i].pid == -1)
	  {
	    printf("Could not find %s !!\n",procname);
	  }
	index[0]++;
      }
      
      //take care of mapping betw PIC actual and physical numbers
      MapPICDaemons();
      
      printf("Mapping PIC Daemons Done..\n");
      
      
      //set PIC default vel and acc
      //for (int v = 0; v < MAXVIRTUALAXES; v++){
//	PICdefaultvel[v] = 60000;
//	PICdefaultacc[v] = 600000;
  //    }

      
      
      //start encoder thread
      Encoder.Start(0);
      
      return 0;
    }
    
//Calibrate a particular axis
     //calibration params (vel and acc) are handled by each MotorCtrl.lib and
     //PIC.lib
 int YAPOCVirtualControl::CalibrateEach(int vnumber){
   if (axismap[vnumber][AXISTYPE] == MEITYPE){
     if (DEBUG) printf("Will calibrate axis %d \n",axismap[vnumber][AXISNUMBER]);

     /*
     if (MEIhead.Calibrate(axismap[vnumber][AXISNUMBER]) == 0){
       double min, max;
       MEIhead.GetLimits (axismap[vnumber][AXISNUMBER], &min, &max);
       MEIhead.SetLimits (axismap[vnumber][AXISNUMBER], min + 1000.0, max - 1000.0);
       return 0;
       
     }
     else
       return -1;
      */
     return 0;
   }
   else if (axismap[vnumber][AXISTYPE] == PICTYPE){
     
     //nt calibrate_axis(byte axis, int verbose);
     //nt calibrate_multi(int len,int *axes, int verbose);
     int actnumber = axismap[vnumber][AXISNUMBER];
    /*
     if (PICheads[pics_axes[actnumber]].calibrate_axis(
						       address_axes[actnumber],0))
       return 0;
     else
       return -1;
    */ 
     
     return 0;
   }
   else
     return -1;
   
 }
    
    //Calibrate all axes in sequence??
    int YAPOCVirtualControl::CalibrateAll(){
      //wil have to figure out the sequence to do this
      //won't need any input
      //for now only calibrate the MEI axes
      if (DEBUG) printf("Calibrating all axes\n");
     
 
     /*
     int axes[8] = {0,1,2,3,4,5,6,7};
      if (MEIhead.Calibrate(nMEIaxes,axes))
      	return -1;
      */
     if (MEIhead.CalibrateAll())
      return -1;
    
     //PIC
     for (int i = 0; i < nPICaxes; i++){
 
       printf("PICheads[%d] performs calibrate axis %d\n",pics_axes[i],address_axes[i]);
	 if (PICheads[pics_axes[i]].CalibrateAxis(address_axes[i]))
	   return -1;
 
     }
      
     return 0;
     
    }
    
//Move one or multiple axes
    int YAPOCVirtualControl::PositionCommand(float *positions, short *statusbits, short uselimit_flag){
        /*NOTE for caller of this function
	 *    * status bits have to be set properly to distinguish axes
	 *    * that have to be moved. only axes to be moved are sent to the appropriate daemons
	 *   */
      
        double MEIpositions[MAXVIRTUALAXES];
        int MEIstatusbits[MAXVIRTUALAXES];
        long PICpositions[MAXVIRTUALAXES];
        int nPICmoves=0;
        int PICaxestobemoved[MAXVIRTUALAXES];
        //long PICvel[MAXVIRTUALAXES];
        //long PICacc[MAXVIRTUALAXES];
      
      

        //fill out a separate array for MEI and PIC
        for (int i = 0; i < naxes; i++){
	  if (DEBUG) printf(" status %d  = %d ",i,statusbits[i]);
	      if (axismap[i][AXISTYPE] == MEITYPE){
		//MEI case
		MEIpositions[axismap[i][AXISNUMBER]] = (double)positions[i];
		MEIstatusbits[axismap[i][AXISNUMBER]] = (int)statusbits[i];
               if (DEBUG) printf("Want to set MEI axis %d to position %f statusbits %d\n",
		                             axismap[i][AXISNUMBER],MEIpositions[i],MEIstatusbits[i]);
	      }
	      else if (axismap[i][AXISTYPE] == PICTYPE){
		      //PIC case
		if (DEBUG) printf("current pos = %f desired pos = %f ->",pos.positions[i],positions[i]);
	
		if (pos.positions[i] == positions[i])
		  if (DEBUG) printf("SAME\n");
		else
		  if (DEBUG) printf("DIFFERENT\n");
		      if (statusbits[i] != OFF && pos.positions[i] != positions[i]){
			
			    //int move_to_pos(byte axis,
			    //long pos,long vel,long acc,int wait_for_reply);
			
			int PICaxisnumber = axismap[i][AXISNUMBER];
			PICaxestobemoved[nPICmoves] = PICaxisnumber;
			PICpositions[nPICmoves++] = (long)positions[i];
	
		      }
			
	      }
	      else
	          return -1;
	}
        //execute commands
      
      
      if (DEBUG) printf("Executing PositionCommand \n");
           
      int MEIdone = 1;
      if (DEBUG) printf("Sending to MEI: MEIpositions (size = %d)\n",nMEIaxes);
      for (int j = 0; j < nMEIaxes; j++){
	if (DEBUG) printf("MEIpositions[%d] = %f \n",j,MEIpositions[j]);
     
	//for all 'valid' move requests, check if previous move on that motor have completed.
	if (MEIstatusbits[axismap[j][AXISNUMBER]] != OFF){
	  double toprint = (double)pos.positions[i];
	  if (MEIdone == 1 && !MEIhead.AxisDone(axismap[j][AXISNUMBER])) 
	    MEIdone = 0;
	}
      }

      
      //only execute positioncommand if all previous moves have been completed
      if (MEIdone == 1){
	if (MEIhead.PositionCommand(MEIpositions,MEIstatusbits,uselimit_flag))
	  return -1;
      }
      else {
	if (DEBUG) printf("Previous MEI move is not finished, dropped.\n");
      }
      if (DEBUG) printf("nPICmoves = %d\n",nPICmoves);
      
      
      for (j = 0; j < nPICmoves; j++){
	  if (DEBUG) printf("PICheads[%d].move addr %d to pos %d\n",pics_axes[PICaxestobemoved[j]],
		 address_axes[PICaxestobemoved[j]],PICpositions[j]);
	  
	//check first if previous move for each axis has been completed
	//if not, ignore
        if (PICheads[pics_axes[PICaxestobemoved[j]]].D_IsMoveDone(
				   address_axes[PICaxestobemoved[j]]) == 1){
	
	    if (PICheads[pics_axes[PICaxestobemoved[j]]].D_MovePosNoWait(
									 address_axes[PICaxestobemoved[j]],ABSOLUTE_MOVE,PICpositions[j],0, uselimit_flag))
	      return -1;
	}
	else {
	  if (DEBUG) printf("Previous move to PIC axis %d is not finished, dropped\n",PICaxestobemoved[j]);
	}
    
      }

      return 0;
    }


     int YAPOCVirtualControl::ZeroEncoder(int axisvnumber){
       
       if (axismap[axisvnumber][AXISTYPE] == MEITYPE){
	 int stat = MEIhead.ZeroAxis(axismap[axisvnumber][AXISNUMBER]);

	 if (stat != 0)
	   {
	     if (DEBUG) printf("MEI Zero Axis... Error code %d!\n", stat);
	     exit(-1);
	   }
	 if (DEBUG) printf("Axis %d encoder is now zero. \n",axismap[axisvnumber][AXISNUMBER]);
       }
       else if (axismap[axisvnumber][AXISTYPE] == PICTYPE){
	 int picaxisnumber = axismap[axisvnumber][AXISNUMBER];
	 PICheads[pics_axes[picaxisnumber]].D_ResetEncoder(address_axes[picaxisnumber]);
	 if (DEBUG) printf("PIC %d Axis %d encoder is now zero. \n",pics_axes[picaxisnumber],address_axes[picaxisnumber]);
       }
       else 
	 return -1;
       
       return 0;
     }
   
     int YAPOCVirtualControl::SetVelocity(float* newvelocity, short* statusbits){
       
       double MEIvels[MAXVIRTUALAXES];
       double MEIaccs[MAXVIRTUALAXES];
       int MEIstatusbits[MAXVIRTUALAXES];
       //fill out a separate array for MEI and PIC
       for (int i = 0; i < naxes; i++){
	 if (axismap[i][AXISTYPE] == MEITYPE){
	   //MEI case
	   MEIvels[axismap[i][AXISNUMBER]] = (double)newvelocity[i];
	   if (statusbits[i] == ON)
	     MEIaccs[axismap[i][AXISNUMBER]] = (double)newvelocity[i] * 20;
	   MEIstatusbits[axismap[i][AXISNUMBER]] = (int)statusbits[i];

	 }
	 else if (axismap[i][AXISTYPE] == PICTYPE){
	   //PIC case
	   if (statusbits[i] == ON){
	     PICheads[pics_axes[axismap[i][AXISNUMBER]]].SetVelocity(address_axes[axismap[i][AXISNUMBER]], (long)newvelocity[i]);
	     
	   }
	   
	 }
	 else
	   return -1;
       }

       //actually set MEI velocity
       if (MEIhead.SetDefaultVelocity(MEIvels, MEIstatusbits)){
	 if (DEBUG) printf("Failed in setting MEI velocities\n");
	 return -1;
       }
       if (MEIhead.SetDefaultAcc(MEIaccs, MEIstatusbits)){
	          if (DEBUG) printf("Failed in setting MEI accelerations\n");
	          return -1;
       }
       
       return 0;
       
     }
   


void main(int argc, char *argv[])
{

  
  //register input port, note that output port is registered by encoder thread
  inCommandPort.Register ("/yapoc/in/command");
 
  //initialize virtualcontrol
  //this involves initializing MEI and PIC client instances as well
   virtualcontrol.Initialize();
    
  
  //continuous loop
  while (1)
    {
      //read command from input port
      inCommandPort.Read();
      YAPOCMessage inputcommand = inCommandPort.Content();

      
      if (DEBUG) printf("Received command %d\n", inputcommand.type);
      
      //expects type to be betw 1-x, else ignored
      if (inputcommand.type > 0 && inputcommand.type <= NCOMMANDTYPES){
	
	
	if (inputcommand.type == CALIBRATE_EACH){
	  if (virtualcontrol.CalibrateEach((int)inputcommand.axisvnumber) == 0)
	    if (DEBUG) printf("Calibrate Each completed\n");
	  else
	    if (DEBUG) printf("Error in Calibrating Each\n");
	}
	
	if (inputcommand.type == CALIBRATE_ALL){
	  if (virtualcontrol.CalibrateAll() == 0)
	    if (DEBUG) printf("Calibrate All completed\n");
	  else
	    if (DEBUG) printf("Error in Calibrating All\n");
	  
	}
	
	if (inputcommand.type == POSITIONCOMMAND){

	  if (virtualcontrol.PositionCommand(inputcommand.params, 
					 inputcommand.statusbits,inputcommand.uselimit_flag) == 0)
	    if (DEBUG) printf("PositionCommand completed\n");
	  else
	    if (DEBUG) printf("Error in sending PositionCommand\n");
	  
	  
	}
	
	if (inputcommand.type == SET_VELOCITY){
	  if (virtualcontrol.SetVelocity(inputcommand.params, inputcommand.statusbits) == 0)
	    if (DEBUG) printf("Set Velocity completed\n");
	  else
	    if (DEBUG) printf("Error in sending Set Velocity\n");
	  
	}
	
	if (inputcommand.type == MEID_RESET){
	  int stat = meid_dsp_reset(virtualcontrol.meidx);
	    if (stat != 0)
	    {
	      if (DEBUG) printf("MEI Reset Failed... Error code %d!\n", stat);
	    }	  
	    else
	    if (DEBUG) printf("MEI Reset Done...\n");
	}
	
	if (inputcommand.type == ZERO_ENCODER){
	  virtualcontrol.ZeroEncoder((int)inputcommand.axisvnumber);

	}
	
	if (inputcommand.type == MEID_CLOSE){
	  if (virtualcontrol.meidx != -1)
	    meid_close (virtualcontrol.meidx);
	  virtualcontrol.meidx = -1;
	  if (DEBUG) printf("MEID is closed\n");
	 exit (0);
	  
	}
	
	if (inputcommand.type == PIC_RESET){
	  for (int p = 0; p < numPICdaemons; p++)
	    virtualcontrol.PICheads[p].D_HardReset();
	  
	}
	
	if (inputcommand.type == SET_PICGAIN){
	  int picaxisnumber = virtualcontrol.axismap[inputcommand.axisvnumber][AXISNUMBER];

	  if (DEBUG) printf("PIC %d motor %d, new gains = %d %d %d %d 255 0 4000 1 0\n",virtualcontrol.pics_axes[picaxisnumber],virtualcontrol.address_axes[picaxisnumber],
		  (int)inputcommand.gains[0], (int)inputcommand.gains[1], (int)inputcommand.gains[2],(int)inputcommand.gains[3]);
	   virtualcontrol.PICheads[virtualcontrol.pics_axes[picaxisnumber]].D_ServoSetGain(virtualcontrol.address_axes[picaxisnumber],
									   (int)inputcommand.gains[0], (int)inputcommand.gains[1], (int)inputcommand.gains[2],
									   (int)inputcommand.gains[3],255,0,4000,1,0);
	  
	}
      }
      else{
	//invalid command
	if (DEBUG) printf("Received INVALID command, thus ignored..\n");
      }
     
    }
}
	 



