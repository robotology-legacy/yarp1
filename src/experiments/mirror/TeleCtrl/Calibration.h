// -----------------------------------
// calibration structure
// -----------------------------------

// calibrate linearly: initialise with hard (robot) limits, then calibrate giving
// soft (glove, ...) values and obtain a linear mapping.

class LinCalibration {

public:
	LinCalibration (double hardMin, double hardMax)
		: _hardMin(hardMin), _hardMax(hardMax), _softMin(0.0), _slope(0.0) { }
	void eval(double softMin, double softMax) {
		_slope = (_hardMax-_hardMin)/(softMax-softMin);
		_softMin = softMin;
	}
	virtual double operator() (double softValue) { return _hardMin + (softValue-_softMin) * _slope; }
protected:
	double _hardMin, _hardMax, _softMin;
	double _slope;
};

// linear calibration with limits: never return more than the upper hard
// limit, and never less than the lower hard limit

class LimitLinCalibration : public LinCalibration {

public:
	LimitLinCalibration (double hardMin, double hardMax) : LinCalibration (hardMin, hardMax) { }
	double operator() (double softValue) {
		double retValue = _hardMin + (softValue-_softMin) * _slope;
		if ( retValue < min(_hardMin,_hardMax) ) {
			return min(_hardMin,_hardMax);
		} else if ( retValue > max(_hardMin,_hardMax) ) {
			return max(_hardMin,_hardMax);
		} else {
			return retValue;
		}
	}

};
