/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran Gonzalez#                      ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: soundidentification.cpp,v 1.23 2004-12-30 10:51:53 beltran Exp $
///

/** 
 * @file soundidentification.cpp Implements a soundprocessing front-end.
 * This consist in a sound parametrization throuth a Short Time Fast Fourier
 * Transform procedure. Later, a MFCC (Mel-Frequency Cepstral Coefficients) is
 * implemented to provide a sound representation with a parameter reduced representation.
 * 
 * @author Carlos Beltran
 * @date 2004-08-24
 */

#include "SoundIdentificationThread.h"

const int   __outSize    = 5;
const char *__baseName   = "/soundidentification/";
const char *__configFile = "sound.ini";
// Initiaization of SoundImagePair static variables
int SoundImagePair::imageWidth = 0;
int SoundImagePair::imageHeight = 0;

/**
 * Some variable for the network connection.
 */
bool _sharedmem = false;
int  _protocol  = YARP_MCAST;

/** 
 * The main function.
 * 
 * @param argc The number of parameters
 * @param argv The buffer of parameters
 * 
 * @return 0 always
 */
int main(int argc, char* argv[]) {

	int iSValue = 15;
	double dDecayFactor = 0.99;
	int learningState = 1;
	YARPString c;
	SoundIdentificationThread _thread;
	_thread.Begin();

	do {
		cin >> c;
		int pos = 0;

		// Get and sets SValue (temporal variable)
		if ( (pos = c.strstr("SValue")) != -1){
			YARPString value = c.substring(pos+6, -1);
			// Is the user asking for the current value?
			if (value.strstr("?") != -1){
				cout << "Current SValue = " << _thread.getSValue() << endl;  
			}
			else{
				iSValue= atoi(value.c_str());
				_thread.setSValue(iSValue);
			}
			continue;
		}

		// Gets and set decaing factor value
		if ( (pos = c.strstr("Decaing")) != -1){
			YARPString value = c.substring(pos+7, -1);
			// Is the user asking for the value
			if (value.strstr("?") != -1){
				 cout << "Current decaing factor = " << _thread.getDecaingFactor() << endl;
			}
			else{
				dDecayFactor = atof(value.c_str());
				_thread.setDecaingFactor(dDecayFactor);
			}
			continue;
		}	

		// Gets and set the learning state
		if ( (pos = c.strstr("Learning")) != -1){
			YARPString value = c.substring(pos+8, -1);
			// Is the user asking for the value
			if (value.strstr("?") != -1){
				 cout << "Current learning state = " << _thread.getLearningState() << endl;
			}
			else{
				learningState = atoi(value.c_str());
				_thread.setLearningState( learningState );
			}
			continue;
		}	

		if ( (pos = c.strstr("SavePairList")) != -1) {
			 YARPString name = c.substring(pos+12, -1);
			 _thread.SavePairList(name);
		}

		if ( (pos = c.strstr("LoadPairList")) != -1) {
			 YARPString name = c.substring(pos+12, -1);
			 _thread.LoadPairList(name);
		}
		
		if ( c != "q!") {
			cout << "Type SValue <value> to set the S value(value must be between 2 and 30)" << endl;
			cout << "Type Decaing <value> to set the Decaing factor(value must be between 0.0 and 1.0)" << endl;
			cout << "Type SValue ? to get the S value" << endl;
			cout << "Type Decaing ? to get the Decaing factor" << endl;
			cout << "Type SavePairList <name> to save the current pair list" << endl;
			cout << "Type LoadPairList <name> to load an stored pair list" << endl;
			cout << "Type q!+return to quit" << endl;
		}
	}
	while (c != "q!");

	_thread.End(-1);
	
	return 0;
}
