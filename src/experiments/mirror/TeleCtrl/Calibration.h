// -----------------------------------
// calibration structure
// -----------------------------------

// general calibration class. you initialise it via the constructor
// and eval(), then you call it with () and it returns you a calibrated value

class Calibration {
public:
	virtual void eval(const double, const double) = 0;
	virtual double operator() (const double) const = 0;
};

// calibrate linearly: initialise with hard (robot) limits, then calibrate giving
// soft (glove, ...) values and obtain a linear mapping.

class LinCalibration : public Calibration {
public:
	LinCalibration (const double hardMin, const double hardMax)
		: _hardMin(hardMin), _hardMax(hardMax), _softMin(0.0), _slope(0.0) { }
	void eval(const double softMin, const double softMax) {
		_slope = (_hardMax-_hardMin)/(softMax-softMin);
		_softMin = softMin;
	}
	virtual double operator() (const double softValue) const {
		return _hardMin + (softValue-_softMin) * _slope;
	}
protected:
	double _hardMin, _hardMax, _softMin;
	double _slope;
};

// linear calibration with limits: never return more than the upper hard
// limit, and never less than the lower hard limit

class LimitLinCalibration : public LinCalibration {
public:
	LimitLinCalibration (const double hardMin, const double hardMax)
		: LinCalibration (hardMin, hardMax) { }
	double operator() (const double softValue) const {
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
