#include "HandControlThread.h"

// -----------------------------------
// thread body
// -----------------------------------

void HandControlThread::Body (void)
{

	// loop until the thread is terminated
	while ( !IsTerminated() ) {

		// set operating frequency
		YARPTime::DelayInSeconds(_streamingFrequency);
	    
		// output to screen
//		cout << "Hand:" << "\t"
//			 << _thumb(_data.gloveData.abduction[0])  << "\t"
//			 << _index(_data.gloveData.index[0])  << "\t"
//			 << _fingers(_data.gloveData.middle[0])  << "\t"
//			 << "       \r";
//		cout.flush();

		// send commands to the hand
        sendPosCmd(
			_thumb(_data.gloveData.abduction[0]),
			_handInit1,
			_index(_data.gloveData.index[0]),
			_handInit3,
			_fingers(_data.gloveData.middle[0]),
			_handInit5
		);

	}
    
	return;

}

// -----------------------------------
// thread mandatory methods
// -----------------------------------

void HandControlThread::calibrate(void)
{

	// calibrate fingers
	// hand open
	int gloveOpen[3];
	cout << "(hand calibration 1/2) Open your hand and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
    gloveOpen[0] = _data.gloveData.abduction[0];
    gloveOpen[1] = _data.gloveData.index[0];
    gloveOpen[2] = _data.gloveData.middle[0];
	// hand close
	int gloveClose[3];
	cout << "(hand calibration 2/2) Close your hand and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
    gloveClose[0] = _data.gloveData.abduction[0];
    gloveClose[1] = _data.gloveData.index[0];
    gloveClose[2] = _data.gloveData.middle[0];
	// calibrate fingers
	_thumb.eval(gloveOpen[0],gloveClose[0]);
	_index.eval(gloveOpen[1],gloveClose[1]);
	_fingers.eval(gloveOpen[2],gloveClose[2]);

}

void HandControlThread::initialise(void)
{

	return;

}

void HandControlThread::shutdown(void)
{

	// reset hand position
	sendPosCmd(0, 0, 0, 0, 0, 0);

}

void HandControlThread::sendPosCmd(double dof1, double dof2, double dof3, double dof4, double dof5, double dof6)
{

return;

    YVector handCmd(6);
    YARPBabyBottle tmpBottle;
    tmpBottle.setID(YBVMotorLabel); 

    // send command to the hand
    handCmd(1) = dof1;
    handCmd(2) = dof2;
    handCmd(3) = dof3;
    handCmd(4) = dof4;
    handCmd(5) = dof5;
    handCmd(6) = dof6;

    tmpBottle.reset();
    tmpBottle.writeVocab(YBVocab(YBVHandNewCmd));
    tmpBottle.writeYVector(handCmd);

    _sema.Wait();
	_outPort.Content() = tmpBottle;
    _outPort.Write(true);
	_sema.Post();

}
