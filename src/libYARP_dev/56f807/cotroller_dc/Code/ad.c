/*
 * ad.c
 *	reading from the AD converter.
 */

#include "ad.h"
#include "controller.h"

#define IDLE            0              /* IDLE state           */
#define MEASURE         1              /* MESURE state         */
#define CONTINUOUS      2              /* CONTINUOUS state     */
#define SINGLE          3              /* SINGLE state         */

static bool OutFlg;                    /* Measurement finish flag */
volatile byte ad_ModeFlg;	           /* Current state of device */

word _sample_A[3];					   /* 3 channels */

/**
 * on interrupt flags the output ready bit.
 */
#pragma interrupt 
void AD_interruptCC(void)
{
	_sample_A[0] = (getReg(ADCA_ADRSLT0)) << 1;
	_sample_A[1] = (getReg(ADCA_ADRSLT1)) << 1;
	_sample_A[2] = (getReg(ADCA_ADRSLT2)) << 1;

	setReg(ADCA_ADSTAT, 0x0800);            /* Clear EOSI flag */
	
	OutFlg = TRUE;                       	/* Measured values are available */
	if (!(getRegBit(ADCA_ADCR1, SMODE2))) 
	{
		/* Not running in trigger mode? */
		ad_ModeFlg = IDLE;                	/* Set the bean to the idle mode */
	}
	
	///AD_onEnd();                         	/* If yes then invoke user event */
}

/**
 * starts the acquisition.
 */
static void HWEnDi(void)
{
	if (ad_ModeFlg)  /* Launch measurement? */
	{
		OutFlg = FALSE;                    	  /* Measured values are available */
		/* Trigger mode? */
		if (getRegBit (ADCA_ADCR1, SMODE2)) 
		{ 
			setRegBit (ADCA_ADCR1, SYNC);     /* Use sync input to initiate a conversion */
			clrRegBit (ADCA_ADCR1, STOP);     /* Normal operation mode */
		}
		else 
		{
			/* Set normal operation mode and sync input disabled */
			clrRegBits (ADCA_ADCR1, ADCA_ADCR1_SYNC_MASK | ADCA_ADCR1_STOP_MASK); 
			setRegBit (ADCA_ADCR1, START);    /* Launching of conversion */
		}
	}
	else 
	{
		setRegBit (ADCA_ADCR1, STOP);         /* Stop command issued */
	}
}

/*
 * This method performs one measurement on all channels that
 *  are set in the bean inspector. (Note: If the <number of
 *  conversions> is more than one the conversion of A/D
 *  channels is performed specified number of times.)
 *
 * @param wait waits for result to be ready.
 * @return ERR_OK after a successful sampling, ERR_BUSY if the device is
 * already running a conversion.
 */
byte AD_measure(bool WaitForResult)
{
	if (ad_ModeFlg != IDLE)
		return ERR_BUSY;
		
	/* sequential once mode */
	clrRegBits (ADCA_ADCR1, 0x00);
	
	ad_ModeFlg = MEASURE;
	
	HWEnDi();
	
	/* wait on the interrupt */
	if (WaitForResult)
		while (ad_ModeFlg == MEASURE) {}
	
	return ERR_OK;
}

/*
 * enables triggered sequential mode synchronous with the
 * PWM generation signal.
 */
byte AD_enableIntTrigger(void)
{
	if (ad_ModeFlg != IDLE)             /* Is the device in running mode? */
		return ERR_BUSY;
		
	/// starts sampling in triggered sequential mode
	/// synchro with PWM generation.
	setRegBits (ADCA_ADCR1, 0x04);
	clrRegBits (ADCA_ADCR1, 0x03);
	
	ad_ModeFlg = MEASURE;               /* Set state of device to the measure mode */
	
	HWEnDi();
	return ERR_OK;
}

/*
 * stops the acquisition, disables interrupts.
 * use init to start acquisition again.
 */
byte AD_stopAcquisition(void)
{
	setRegBit (ADCA_ADCR1, STOP);         /* Stop command issued */
	return ERR_OK;
}

/**
 * gets the sampled values if available.
 * @param values is a pointer to an array of three elements.
 * @return ERR_OK if values are available, ERR_NOTAVAIL otherwise.
 */
byte AD_getValue16 (word *values)
{
	AD_DI;
	values[0] = _sample_A[0];
	values[1] = _sample_A[1];
	values[2] = _sample_A[2];
	AD_EI;
	
	return ERR_OK;
#if 0
	if (!OutFlg)                         /* Is measured value(s) available? */
		return ERR_NOTAVAIL;             
		
	*values++ = (getReg(ADCA_ADRSLT0)) << 1;
	*values++ = (getReg(ADCA_ADRSLT1)) << 1;
	*values = (getReg(ADCA_ADRSLT2)) << 1;
	
	return ERR_OK;
#endif
}

/**
 * gets the sampled values if available.
 * @param i is the index of the channel to read 0-2
 * @param values is a pointer to a 16 bit value.
 * @return ERR_OK if values are available, ERR_NOTAVAIL otherwise.
 */
byte AD_getChannel16(byte i, word *value)
{
	if (i < 0 || i > 2)
		return ERR_RANGE;
	AD_DI;
	*value = _sample_A[i];
	AD_EI;
}

/**
 * initializes the AD conversion module.
 */
void AD_init (void)
{
	OutFlg = FALSE;                      /* No measured value */
	ad_ModeFlg = IDLE;                  /* Device isn't running */

	setReg(ADCA_ADCR1, 0x4800);           /* Set control register 1 */
	setReg(ADCA_ADOFS0, 0);               /* Set offset reg. 0 */
	setReg(ADCA_ADOFS1, 0);               /* Set offset reg. 1 */
	setReg(ADCA_ADOFS2, 0);               /* Set offset reg. 2 */
	setReg(ADCA_ADHLMT0, 0x7ff8);         /* Set high limit reg. 0 */
	setReg(ADCA_ADHLMT1, 0x7ff8);         /* Set high limit reg. 1 */
	setReg(ADCA_ADHLMT2, 0x7ff8);         /* Set high limit reg. 2 */
	setReg(ADCA_ADLLMT0, 0);              /* Set low limit reg. 0 */
	setReg(ADCA_ADLLMT1, 0);              /* Set low limit reg. 1 */
	setReg(ADCA_ADLLMT2, 0);              /* Set low limit reg. 2 */
	setReg(ADCA_ADZCSTAT, 0xff);          /* Clear zero crossing status flags */
	setReg(ADCA_ADLSTAT, 0xffff);         /* Clear high and low limit status */
	setReg(ADCA_ADSTAT, 0x800);           /* Clear EOSI flag */
	setReg(ADCA_ADSDIS, 0xf8);            /* Enable/disable of samples */
	setReg(ADCA_ADLST1, 0x105);           /* Set ADC channel list reg. */
	setReg(ADCA_ADZCC, 0);                /* Set zero crossing control reg. */
	setReg(ADCA_ADCR2, 0xf);              /* Set prescaler */

	_sample_A[0] = 0;
	_sample_A[1] = 0;
	_sample_A[2] = 0;
	
	HWEnDi();                             /* Enable/disable device according to the status flags */
}

/**
 * event like function called from within the ISR.
 */
#pragma interrupt called
void AD_onEnd (void)
{
	/* Write your code here ... */
}


