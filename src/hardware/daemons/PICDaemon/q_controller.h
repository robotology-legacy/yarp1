
#ifndef COMMONDEF
#define COMMONDEF


//Common Definitions
#define MAXSIOERROR 2

//Define PIC baud rate divisors
#define	PB19200		63
#define	PB57600		20
#define	PB115200	10

//Module type definitions:
#define	SERVOMODTYPE	0
#define	ADCMODTYPE		1
#define	IOMODTYPE		2
#define	STEPMODTYPE		3

#define CKSUM_ERROR		0x02	//Checksum error bit in status byte

#define MAXNUMMOD	33

#define INVALID_HANDLE_VALUE -1 


//Servo Module Command set:
#define	RESET_POS	  0x00	//Reset encoder counter to 0 (0 bytes)
#define	SET_ADDR	  0x01	//Set address and group address (2 bytes)
#define	DEF_STAT	  0x02	//Define status items to return (1 byte)
#define	READ_STAT	  0x03	//Read value of current status items
#define	LOAD_TRAJ  	  0x04	//Load trahectory date (1 - 14 bytes)
#define START_MOVE	  0x05	//Start pre-loaded trajectory (0 bytes)
#define SET_GAIN	  0x06  //Set servo gain and control parameters (13 or 14)
#define	STOP_MOTOR 	  0x07	//Stop motor (1 byte)
#define	IO_CTRL		  0x08	//Define bit directions and set output (1 byte)
#define SET_HOMING	  0x09  //Define homing mode (1 byte)
#define	SET_BAUD	  0x0A 	//Set the baud rate (1 byte)
#define CLEAR_BITS	  0x0B  //Save current pos. in home pos. register (0 bytes)
#define SAVE_AS_HOME  0x0C	//Store the input bytes and timer val (0 bytes)
#define ADD_PATHPOINT 0x0D  //Adds path points for path mode
#define	NOP			  0x0E	//No operation - returns prev. defined status (0 bytes)
#define HARD_RESET	  0x0F	//RESET - no status is returned

//Servo Module STATUSITEMS bit definitions:
#define	SEND_POS	  0x01	//4 bytes data
#define	SEND_AD		  0x02	//1 byte
#define	SEND_VEL	  0x04	//2 bytes
#define SEND_AUX	  0x08	//1 byte
#define SEND_HOME	  0x10	//4 bytes
#define SEND_ID		  0x20	//2 bytes
#define SEND_PERROR	  0x40  //2 bytes
#define SEND_NPOINTS  0x80  //1 byte

//Servo Module LOAD_TRAJ control byte bit definitions:
#define	LOAD_POS	  0x01	//+4 bytes
#define LOAD_VEL	  0x02	//+4 bytes
#define	LOAD_ACC	  0x04	//+4 bytes
#define LOAD_PWM	  0x08	//+1 byte
#define ENABLE_SERVO  0x10  //1 = servo mode, 0 = PWM mode
#define VEL_MODE	  0x20	//1 = velocity mode, 0 = trap. position mode
#define REVERSE		  0x40  //1 = command neg. PWM or vel, 0 = positive
#define MOVE_REL	  0x40  //1 = move relative, 0 = move absolute
#define START_NOW	  0x80  //1 = start now, 0 = wait for START_MOVE command

//Servo Module STOP_MOTOR control byte bit definitions:
#define	AMP_ENABLE	  0x01	//1 = raise amp enable output, 0 = lower amp enable
#define MOTOR_OFF	  0x02	//set to turn motor off
#define STOP_ABRUPT   0x04	//set to stop motor immediately
#define STOP_SMOOTH	  0x08  //set to decellerate motor smoothly
#define STOP_HERE	  0x10	//set to stop at position (4 add'l data bytes required)
#define ADV_FEATURE   0x20  //enable features in ver. CMC

//Servo Module IO_CTRL control byte bit definitions:
#define SET_OUT1	  0x01	//1 = set limit 1 output, 0 = clear limit 1 output
#define SET_OUT2	  0x02	//1 = set limit 2 output, 0 = clear limit 2 output
#define IO1_IN		  0x04	//1 = limit 1 is an input, 0 = limit 1 is an output
#define IO2_IN		  0x08	//1 = limit 2 is an input, 0 = limit 2 is an output
#define	FAST_PATH	  0x40  //0 = 30 or 60 Hz path execution, 1 = 60 or 120 Hz

//Servo Module SET_HOMING control byte bit definitions:
#define ON_LIMIT1	  0x01	//home on change in limit 1
#define ON_LIMIT2	  0x02	//home on change in limit 2
#define HOME_MOTOR_OFF  0x04  //turn motor off when homed
#define ON_INDEX	  0x08	//home on change in index
#define HOME_STOP_ABRUPT 0x10 //stop abruptly when homed
#define HOME_STOP_SMOOTH 0x20 //stop smoothly when homed
#define ON_POS_ERR	  0x40	//home on excessive position error
#define	ON_CUR_ERR	  0x80  //home on overcurrent error

//Servo Module RESET_POS control byte bit definitions:
//(if no control byte is used, reset is absolute)
#define REL_HOME 	  0x01	//Reset position relative to current home position

//Servo Module ADD_PATHPOINT frequency definitions
#define P_30HZ		  30	//30 hz path resolution
#define P_60HZ		  60    //60 hs path resolution
#define P_120HZ		  120   //120 hs path resolution

//Servo Module Status byte bit definitions:
#define MOVE_DONE	  0x01	//set when move done (trap. pos mode), when goal
							//vel. has been reached (vel mode) or when not servoing
#define CKSUM_ERROR	  0x02	//checksum error in received command
#define OVERCURRENT	  0x04	//set on overcurrent condition (sticky bit)
#define POWER_ON	  0x08	//set when motor power is on
#define POS_ERR		  0x10	//set on excess pos. error (sticky bit)
#define LIMIT1		  0x20	//value of limit 1 input
#define LIMIT2		  0x40	//value of limit 2 input
#define HOME_IN_PROG  0x80  //set while searching for home, cleared when home found

//Servo Module Auxilliary status byte bit definitions:
#define INDEX		  0x01	//value of the encoder index signal
#define POS_WRAP	  0x02	//set when 32 bit position counter wraps around
							//  (sticky bit)
#define SERVO_ON	  0x04	//set when position servo is operating
#define ACCEL_DONE	  0x08	//set when acceleration portion of a move is done
#define SLEW_DONE	  0x10  //set when slew portion of a move is done
#define SERVO_OVERRUN 0x20  //set if servo takes longer than the specified
							//servo period to execute
#define PATH_MODE	  0x40  //path mode is enabled (v.5)

// IO Defines

//IO Module Command set:
#define	SET_IO_DIR	  0x00	//Set direction of IO bits (2 data bytes)
#define	SET_ADDR	  0x01	//Set address and group address (2 bytes)
#define	DEF_STAT	  0x02	//Define status items to return (1 byte)
#define	READ_STAT	  0x03	//Read value of current status items
#define	SET_PWM   	  0x04	//Immediatley set PWM1 and PWM2 (2 bytes)
#define SYNCH_OUT	  0x05	//Output prev. stored PWM & output bytes (0 bytes)
#define SET_OUTPUT	  0x06  //Immediately set output bytes
#define	SET_SYNCH_OUT 0x07	//Store PWM & outputs for synch'd output (4 bytes)
#define	SET_TMR_MODE  0x08	//Set the counter/timer mode (1 byte)
//Not used			  0x09
#define	SET_BAUD	  0x0A 	//Set the baud rate (1 byte)
//Not used			  0x0B
#define SYNCH_INPUT	  0x0C	//Store the input bytes and timer val (0 bytes)
//Not used			  0x0D
#define	NOP			  0x0E	//No operation - returns prev. defined status (0 bytes)
#define HARD_RESET	  0x0F	//RESET - no status is returned

//IO Module STATUSITEMS bit definitions
#define	SEND_INPUTS	  0x01	//2 bytes data
#define	SEND_AD1	  0x02	//1 byte
#define	SEND_AD2	  0x04	//1 byte
#define SEND_AD3	  0x08	//1 byte
#define SEND_TIMER	  0x10	//4 bytes
#define SEND_ID		  0x20	//2 bytes
#define	SEND_SYNC_IN  0x40	//2 bytes
#define	SEND_SYNC_TMR 0x80	//4 bytes

//IO Module Timer mode definitions
//Timer mode and resolution may be OR'd together
#define	OFFMODE		  0x00
#define	COUNTERMODE	  0x03
#define	TIMERMODE	  0x01
#define	RESx1		  0x00
#define RESx2		  0x10
#define RESx4		  0x20
#define RESx8		  0x30

// Step definitino

//Step Module Command set:
#define	RESET_POS	  0x00	//Reset encoder counter to 0 (0 bytes)
#define	SET_ADDR	  0x01	//Set address and group address (2 bytes)
#define	DEF_STAT	  0x02	//Define status items to return (1 byte)
#define	READ_STAT	  0x03	//Read value of current status items
#define	LOAD_TRAJ  	  0x04	//Load trajectory data
#define START_MOVE	  0x05	//Start pre-loaded trajectory (0 bytes)
#define SET_PARAM	  0x06  //Set operating parameters (6 bytes)
#define	STOP_MOTOR 	  0x07	//Stop motor (1 byte)
#define	SET_OUTPUTS	  0x08	//Set output bits (1 byte)
#define SET_HOMING	  0x09  //Define homing mode (1 byte)
#define	SET_BAUD	  0x0A 	//Set the baud rate (1 byte)
#define RESERVED	  0x0B  //
#define SAVE_AS_HOME  0x0C	//Store the input bytes and timer val (0 bytes)
#define NOT_USED	  0x0D
#define	NOP			  0x0E	//No operation - returns prev. defined status (0 bytes)
#define HARD_RESET	  0x0F	//RESET - no status is returned

//Step Module STATUSITEMS bit definitions:
#define	SEND_POS	  0x01	//4 bytes data
#define	SEND_AD		  0x02	//1 byte
#define	SEND_ST		  0x04	//2 bytes
#define SEND_INBYTE	  0x08	//1 byte
#define SEND_HOME	  0x10	//4 bytes
#define SEND_ID		  0x20	//2 bytes

//Step Module LOAD_TRAJ control byte bit definitions:
#define	LOAD_POS	  0x01	//+4 bytes
#define LOAD_SPEED	  0x02	//+1 bytes
#define	LOAD_ACC	  0x04	//+1 bytes
#define LOAD_ST		  0x08	//+3 bytes
#define STEP_REV      0x10  //reverse dir
#define START_NOW	  0x80  //1 = start now, 0 = wait for START_MOVE command

//Step Module SET_PARAM operating mode byte bit definitions:
#define	SPEED_8X	  0x00	//use 8x timing
#define	SPEED_4X	  0x01	//use 4x timing
#define	SPEED_2X	  0x02	//use 2x timing
#define	SPEED_1X	  0x03	//use 1x timing
#define IGNORE_LIMITS 0x04	//Do not stop automatically on limit switches
#define	IGNORE_ESTOP  0x08  //Do not stop automatically on e-stop
#define ESTOP_OFF  0x10  //Stop abrupt on estop or limit switch

//Step Module STOP_MOTOR control byte bit definitions:
#define	AMP_ENABLE	  0x01	//1 = raise amp enable output, 0 = lower amp enable
#define STOP_ABRUPT   0x04	//set to stop motor immediately
#define STOP_SMOOTH	  0x08  //set to decellerate motor smoothly

//Step Module SET_HOMING control byte bit definitions:
#define ON_LIMIT1	  0x01	//home on change in limit 1
#define ON_LIMIT2	  0x02	//home on change in limit 2
#define HOME_MOTOR_OFF 0x04  //turn motor off when homed
#define ON_HOMESW	  0x08	//home on change in index
#define HOME_STOP_ABRUPT 0x10 //stop abruptly when homed
#define HOME_STOP_SMOOTH 0x20 //stop smoothly when homed

//Step Module Status byte bit definitions:
#define MOTOR_MOVING  0x01	//Set when motor is moving
#define CKSUM_ERROR	  0x02	//checksum error in received command
#define AMP_ENABLED	  0x04	//set amplifier is enabled
#define POWER_ON	  0x08	//set when motor power is on
#define AT_SPEED	  0x10	//set on excess pos. error (sticky bit)
#define VEL_MODE 	  0x20	//set when in velocity profile mode
#define TRAP_MODE	  0x40	//set when in trap. profile mode
#define HOME_IN_PROG  0x80  //set while searching for home, cleared when home found

//Step Module Input byte bit definitions:
#define	ESTOP		0x01	//emergency stop input
#define	AUX_IN1		0x02	//auxilliary input #1
#define	AUX_IN2		0x02	//auxilliary input #2
#define FWD_LIMIT	0x04	//forward limit switch
#define REV_LIMIT  	0x08	//reverse limit switch
#define HOME_SWITCH 0x10	//homing limit switch




//Common Structures

typedef unsigned char byte;

typedef struct _NMCMOD {
	byte	modtype;		//module type
    byte	modver;			//module version number
	byte	statusitems;	//definition of items to be returned
	byte	stat;  			//status byte
    byte	groupaddr;		//current group address
    char	groupleader;	//TRUE if group leader
    void *	p;				//pointer to specific module's data structure
    } NMCMOD;

//--------------------- ADC Module specific stuff --------------------------
typedef struct _ADCMOD {
    //******  Move all this data to the NMCMOD structure *******
	short int ad0;	//definition of items to be returned
	short int ad1;
	short int ad2;
	short int ad3;
	short int ad4;
	short int ad5;
    } ADCMOD;


//--------------------- Servo Module specific stuff ---------------------------
typedef	struct _GAINVECT {
    short int	kp;				//gain values
    short int	kd;
    short int	ki;
    short int	il;
    byte		ol;
    byte		cl;
    short int	el;
    byte		sr;
    byte		dc;
	} GAINVECT;

typedef struct _SERVOMOD {
    long		pos;     		//current position
    byte		ad;				//a/d value
    short int	vel;            //current velocity
    byte		aux;            //auxilliary status byte
    long		home;           //home position
    short int	perror;			//position error
    byte		npoints;		//number of points in path buffer

    //The following data is stored locally for reference
    long		cmdpos;			//last commanded position
    long		cmdvel;			//last commanded velocity
    long		cmdacc;			//last commanded acceleration
    byte		cmdpwm;			//last commanded PWM value
    GAINVECT    gain;
    long		stoppos;		//motor stop position (used by stop command)
    byte		stopctrl;		//stop control byte
    byte		movectrl;		//load_traj control byte
    byte		ioctrl;			//I/O control byte
    byte		homectrl;		//homing control byte
    byte		ph_adv;			//phase advance (for ss-drive modules)
    byte		ph_off;			//phase offset (for ss-drive modules)
    long		last_ppoint;	//last path point specified
    } SERVOMOD;


//--------------------- IO Module specific stuff ---------------------------
typedef struct _IOMOD {
    short int	inbits;			//input bits
    byte		ad1;			//A/D input bytes
    byte		ad2;
    byte		ad3;
    unsigned long timer; 		//timer value
    short int	inbits_s;		//synchronized input bytes
    unsigned long timer_s;		//synchronized timer value
    //The following data is stored locally for reference
    byte		pwm1;			//current PWM output values
    byte		pwm2;
    byte		timermode;		//current timer mode
    short int	bitdir;			//current bit direction values
    short int	outbits;		//current output byte values
    } IOMOD;


//--------------------- END IO Module specific stuff ------------------------


//--------------------- Stepper Module specific stuff ---------------------------
typedef struct _STEPMOD {
    long		pos;     		//current position
    byte		ad;				//a/d value
    unsigned short int	st;          	//current step time
    byte		inbyte;         //input bits
    long		home;           //home position

    //The following data is stored locally for reference
    long		cmdpos;			//last commanded position
    byte		cmdspeed;		//last commanded speed
    byte		cmdacc;			//last commanded acceleration
    short int	cmdst;			//last commanded step time
    byte		min_speed;		//minimum running speed
    byte		stopctrl;		//stop control byte
    byte		outbyte;		//output bits
    byte		homectrl;		//homing control byte
    byte		ctrlmode;		//operating control mode byte
    byte		run_pwm;		//pwm for running current limit
    byte		hold_pwm;		//pwm for holding current limit
    byte		therm_limit;		//thermal limit
    } STEPMOD;

#endif

#if !defined _QNXMC
#define _QNXMC

class Cqnxmc
{
public:
	
	
	Cqnxmc();
	virtual ~Cqnxmc();
private:
	//---------------------------------------------------------------------------
	//Global data
	
	NMCMOD mod[MAXNUMMOD]; 	//Array of modules
	int nummod;				//start off with no modules
	char cstr[20];			//use global command string
	int ComPort;
	int SioError;
	unsigned int BaudRate;
public:
	
	//Common functions
	//-------------------------------------------
	
	//Function prototypes:
	ADCMOD * AdcNewMod();
	char AdcGetStat(byte addr);
	int SimpleMsgBox(char *msgstr);
	//Initialization and shutdown
	//extern "C" "C" WINAPI __declspec(dllexport) int NmcInit(char *portname, unsigned int baudrate);
	int  NmcInit(char *portname, unsigned int baudrate);
	void InitVars(void);
	char NmcSendCmd(byte addr, byte cmd, char *datastr, byte n, byte stataddr);
	void FixSioError(byte addr);
	void NmcShutdown(void);
	
	//Module type independant commands (supported by all module types)
	char NmcSetGroupAddr(byte addr, byte groupaddr, char leader);
	char NmcDefineStatus(byte addr, byte statusitems);
	char NmcReadStatus(byte addr, byte statusitems);
	char NmcSynchOutput(byte groupaddr, byte leaderaddr);
	char NmcChangeBaud(byte groupaddr, unsigned int baudrate);
	char NmcSynchInput(byte groupaddr, byte leaderaddr);
	char NmcNoOp(byte addr);
	char NmcHardReset(byte addr);
	
	//Retrieve module type independant data from a module's data structure
	byte NmcGetStat(byte addr);
	byte NmcGetStatItems(byte addr);
	byte NmcGetModType(byte addr);
	byte NmcGetModVer(byte addr);
	byte NmcGetGroupAddr(byte addr);
	char NmcGroupLeader(byte addr);
	
	
	//-------------------------------------------
	
	//Servo module function prototypes:
	SERVOMOD * ServoNewMod();
	char ServoGetStat(byte addr);
	long ServoGetPos(byte addr);
	byte ServoGetAD(byte addr);
	short int ServoGetVel(byte addr);
	byte ServoGetAux(byte addr);
	long ServoGetHome(byte addr);
	short int ServoGetPError(byte addr);
	byte ServoGetNPoints(byte addr);
	long ServoGetCmdPos(byte addr);
	long ServoGetCmdVel(byte addr);
	long ServoGetCmdAcc(byte addr);
	long ServoGetStopPos(byte addr);
	byte ServoGetCmdPwm(byte addr);
	byte ServoGetMoveCtrl(byte addr);
	byte ServoGetStopCtrl(byte addr);
	byte ServoGetHomeCtrl(byte addr);
	byte ServoGetIoCtrl(byte addr);
	byte ServoGetPhAdv(byte addr);
	byte ServoGetPhOff(byte addr);
	void ServoGetGain(byte addr, short int * kp, short int * kd, short int * ki,
		short int * il, byte * ol, byte * cl, short int * el,
		byte * sr, byte * dc);
	char ServoSetGain(byte addr, short int kp, short int kd, short int ki,
		short int il, byte ol, byte cl, short int el,
		byte sr, byte dc);
	char ServoResetPos(byte addr);
	char ServoResetRelHome(byte addr);
	char ServoClearBits(byte addr);
	char ServoStopMotor(byte addr, byte mode);
	char ServoSetIoCtrl(byte addr, byte mode);
	char ServoLoadTraj(byte addr, byte mode, long pos, long vel, long acc, byte pwm);
	void ServoInitPath(byte addr);
	char ServoAddPathpoints(byte addr, int npoints, long *path, int freq);
	char ServoStartPathMode(byte groupaddr, byte groupleader);
	char ServoSetHoming(byte addr, byte mode);
	
	char ServoSetPhase(byte addr, int padvance, int poffset, int maxpwm);
	
	
	//IO Functions
	
	IOMOD * IoNewMod();
	char IoGetStat(byte addr);
	char IoInBitVal(byte addr, int bitnum);
	char IoInBitSVal(byte addr, int bitnum);
	char IoOutBitVal(byte addr, int bitnum);
	char IoGetBitDir(byte addr, int bitnum);
	byte IoGetADCVal(byte addr, int channel);
	byte IoGetPWMVal(byte addr, int channel);
	unsigned long IoGetTimerVal(byte addr);
	unsigned long IoGetTimerSVal(byte addr);
	byte IoGetTimerMode(byte addr);
	
	char IoSetOutBit(byte addr, int bitnum);
	char IoClrOutBit(byte addr, int bitnum);
	char IoBitDirIn(byte addr, int bitnum);
	char IoBitDirOut(byte addr, int bitnum);
	char IoSetPWMVal(byte addr, byte pwm1, byte pwm2);
	char IoSetTimerMode(byte addr, byte tmrmode);
	char IoSetSynchOutput(byte addr, short int outbits, byte pwm1, byte pwm2);
	
	//Step module function prototypes:
	STEPMOD * StepNewMod();
	char StepGetStat(byte addr);
	long StepGetPos(byte addr);
	byte StepGetAD(byte addr);
	unsigned short int StepGetStepTime(byte addr);
	byte StepGetInbyte(byte addr);
	long StepGetHome(byte addr);
	long StepGetCmdPos(byte addr);
	byte StepGetCmdSpeed(byte addr);
	byte StepGetCmdAcc(byte addr);
	unsigned short int StepGetCmdST(byte addr);
	byte StepGetMinSpeed(byte addr);
	byte StepGetOutputs(byte addr);
	byte StepGetCtrlMode(byte addr);
	byte StepGetRunCurrent(byte addr);
	byte StepGetHoldCurrent(byte addr);
	byte StepGetThermLimit(byte addr);
	byte StepGetHomeCtrl(byte addr);
	byte StepGetStopCtrl(byte addr);
	char StepSetParam(byte addr, byte mode,
		byte minspeed, byte runcur, byte holdcur, byte thermlim);
	char StepLoadTraj(byte addr, byte mode,
		long pos, byte speed, byte acc, float raw_speed);
	char StepResetPos(byte addr);
	char StepStopMotor(byte addr, byte mode);
	char StepSetOutputs(byte addr, byte outbyte);
	char StepSetHoming(byte addr, byte mode);
	
	
};

#endif 

