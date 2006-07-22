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
//			 << _thumb0(_data.gloveData.thumb[0])    << "\t"
//			 << _thumb1(_data.gloveData.thumb[1])    << "\t"
//			 << _index0(_data.gloveData.index[0])    << "\t"
//			 << _index1(_data.gloveData.index[1])    << "\t"
//			 << _fingers0(_data.gloveData.middle[0]) << "\t"
//			 << _fingers1(_data.gloveData.middle[1])
//			 << "       \r";
//		cout.flush();

		// send commands to the hand
        sendPosCmd(
			_thumb0(_data.gloveData.thumb[0]),
			_thumb1(_data.gloveData.thumb[1]),
			_index0(_data.gloveData.index[0]),
			_index1(_data.gloveData.index[1]),
			_fingers0(_data.gloveData.middle[0]),
			_fingers1(_data.gloveData.middle[1])
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
	int gloveOpen[6];
	cout << "(hand calibration 1/2) Open your hand and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
    gloveOpen[0] = _data.gloveData.thumb[0];
    gloveOpen[1] = _data.gloveData.thumb[1];
    gloveOpen[2] = _data.gloveData.index[0];
    gloveOpen[3] = _data.gloveData.index[1];
    gloveOpen[4] = _data.gloveData.middle[0];
    gloveOpen[5] = _data.gloveData.middle[1];
	// hand close
	int gloveClose[6];
	cout << "(hand calibration 2/2) Close your hand and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
    gloveClose[0] = _data.gloveData.thumb[0];
    gloveClose[1] = _data.gloveData.thumb[1];
    gloveClose[2] = _data.gloveData.index[0];
    gloveClose[3] = _data.gloveData.index[1];
    gloveClose[4] = _data.gloveData.middle[0];
    gloveClose[5] = _data.gloveData.middle[1];
	// calibrate fingers
	_thumb0.eval(gloveOpen[0],gloveClose[0]);
	_thumb1.eval(gloveOpen[1],gloveClose[1]);
	_index0.eval(gloveOpen[2],gloveClose[2]);
	_index1.eval(gloveOpen[3],gloveClose[3]);
	_fingers0.eval(gloveOpen[4],gloveClose[4]);
	_fingers1.eval(gloveOpen[5],gloveClose[5]);

}

void HandControlThread::initialise(void)
{

	sendPosCmd(_handInit0, _handInit1, _handInit2, _handInit3, _handInit4, _handInit5);

}

void HandControlThread::shutdown(void)
{

	sendPosCmd(_handInit0, _handInit1, _handInit2, _handInit3, _handInit4, _handInit5);

}

void HandControlThread::sendPosCmd(double dof1, double dof2, double dof3, double dof4, double dof5, double dof6)
{

	// this function needs DEGREES; it converts them to radiants

    YVector handCmd(6);
    YARPBabyBottle tmpBottle;
    tmpBottle.setID(YBVMotorLabel); 

    // send command to the hand
    handCmd(1) = dof1*DegRad;
    handCmd(2) = dof2*DegRad;
    handCmd(3) = dof3*DegRad;
    handCmd(4) = dof4*DegRad;
    handCmd(5) = dof5*DegRad;
    handCmd(6) = dof6*DegRad;

    tmpBottle.reset();
    tmpBottle.writeVocab(YBVocab(YBVHandNewCmd));
    tmpBottle.writeYVector(handCmd);

	if ( _enabled ) {
	    _sema.Wait();
		_outPort.Content() = tmpBottle;
	    _outPort.Write(true);
		_sema.Post();
	}

}
