#ifndef __TirednessControlCLASS__
#define __TirednessControlCLASS__

// Implement the following:
// TirednessControl = sum k*(V-Vd)

class TirednessControl
{
public:
	TirednessControl(double vmax, double vd, double rate = 40, double t_max = 2)
	{
		_vmax = vmax;
		_vd = vd;
		_k = 1/(t_max*60*(vmax-vd));
		_rate = rate/1000;
		_tiredness = 0.0;
		_heated = false;
		_time = 0;
		_on = true;

		_max = 0.8;
		_min = 0.0;

		_forcedResting = false;
	}

	~TirednessControl(){}

	inline int add(double v)
	{
		if (_on) {
			_time++;
			_tiredness += (fabs(v)-_vd)*_k*_rate;
			if (_tiredness <= 0.0)
				_tiredness = 0.0;

			if (_tiredness >= _max)
				_heated = true;
			
			if (_tiredness <= _min)
				_heated = false;
		}

		return 0;
	}

	inline bool low()
	{ return (!_heated && !_forcedResting); }

	inline bool high()
	{ return (_heated || _forcedResting); }

	inline int getTime()
	{ return _time; }

	inline double getSum()
	{ return _tiredness; }

	inline void forceResting(bool f=true)
	{
		_forcedResting = f;
	}

	void start()
	{ _on = true; }

	void stop()
	{ _on = false; }

	// reset (or set to a specified value)
	int reset(double t = 0.0)
	{
		_time = 0;
		_tiredness = t;
		return 0;
	}

private:
	double _rate;
	double _k;
	double _max;
	double _min;
	double _tiredness;
	double _vmax;
	double _vd;
	bool _heated;
	int _time;
	bool _on;
	bool _forcedResting;
};

#endif