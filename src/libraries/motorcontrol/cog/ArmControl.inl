//
// ArmControl.inl
//

//
// Class __positioncontrol
//

inline void __positioncontrol::ComputeErrors (void)
{
	// use a different strategy for joint 0,1,2.

	// takes m_command and computes the error(s).
	for (int i = 3; i < NJOINTS; i++)
	{
		m_old_error[i] = m_error[i];

		m_error[i] = m_command[i] - m_angle[i];
		m_derror[i] = m_error[i] - m_old_error[i];
		m_ierror[i] += m_error[i];

		// clipping of the integral.
		if (m_ierror[i] > m_gains[i][INTEGRAL_LIMIT])
			m_ierror[i] = m_gains[i][INTEGRAL_LIMIT];
		else
		if (m_ierror[i] < -m_gains[i][INTEGRAL_LIMIT])
			m_ierror[i] = -m_gains[i][INTEGRAL_LIMIT];

		// filter the derivative component.
		m_derror[i] = m_alpha * m_derror[i] + (1.0 - m_alpha) * m_oldfiltervalues[i];
		m_oldfiltervalues[i] = m_derror[i];
	}
}

//
// computes the PID controller given the error(s).
// THIS ALSO SENDS THE RESULT TO THE MEI. (mei_set_command).
//
//
// WARNING: we might be running out of time (5ms) the search over the vector might be
//	getting things slow.
//
inline void __positioncontrol::ComputeController(YARPArmControl& control)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		if (m_t_calibrated[i] && m_calibrated[i] && m_enabled[i])
		{
			switch (m_lastcommand[i])
			{
			case COMMAND_NONE:
			case COMMAND_SETPOS:
			case COMMAND_MOVETO:
			default:
				{
					int meidx = control.meidx0;
					int joint = control.get_mei_info (i, &meidx);

					if (meid_frames_left (meidx, joint) == FALSE)
						meid_v_move (meidx, joint, 0.0, m_accel_setpoint[i]);
				}
				break;

			case COMMAND_VEL:
				// simply check the limits against the actual position.
				// m_speed is the velocity command.
				int meidx = control.meidx0;
				int joint = control.get_mei_info (i, &meidx);

				// check limits.
				if (m_angle[i] >= m_limits_max[i] && m_speed[i] > 0)
				{
					m_2b_speed[i] = m_speed[i] = 0.0;
					m_intrajectory[i] = false;
					m_lastcommand[i] = COMMAND_NONE;
				}
				else
				if (m_angle[i] <= m_limits_min[i] && m_speed[i] < 0)
				{
					m_2b_speed[i] = m_speed[i] = 0.0;
					m_intrajectory[i] = false;
					m_lastcommand[i] = COMMAND_NONE;
				}

				if (meid_frames_left (meidx, joint) == FALSE)
					meid_v_move (meidx, joint, m_speed[i], m_accel_setpoint[i]);

				break;
			} /* case */
		} /* if enabled */
	}	/* loop over joints */

	// gravity compensation for joint 3.
	if (m_gravity.IsLearning())
	{
		if (m_gravity.StandingStable (m_angle))
		{
			double tmp =
 					m_gains[3][PROPORTIONAL] * m_error[3] +
					m_gains[3][DERIVATIVE] * m_derror[3] +
					m_gains[3][INTEGRAL] * m_ierror[3];

			m_gravity.Add (m_angle[3], tmp);
		}
	}

	for (i = 3; i < NJOINTS; i++)
	{
		if (m_t_calibrated[i] && m_calibrated[i] && m_enabled[i])
		{
			double tmp = 
				m_gains[i][PROPORTIONAL] * m_error[i] +
				m_gains[i][DERIVATIVE] * m_derror[i] +
				m_gains[i][INTEGRAL] * m_ierror[i];

			if (m_gravity.IsLoaded())
			{
				// temporary.
				if (i == 3 && m_gravity.IsActive())
				{
					double y = m_gravity.Nearest(m_angle[3]);
					tmp += .9 * y;
				}
			}

			// tmp is a torque here.
			int meidx = control.meidx0;
			int joint = control.get_mei_info (i, &meidx);

			tmp += m_torque_zero[i];

			meid_set_command(meidx, joint, tmp);
		}
	}
}

// complete w/ trajectory generation.
inline void __positioncontrol::ComputeTrajectory(void)
{
	// Allows velocity control but no trajectory generation is included.
	// Position control istantaneously move the arm to the new position.
	//
	// for joint 0,1,2 use a different strategy.

	for (int i = 3; i < NJOINTS; i++)
	{
		if (m_t_calibrated[i] && m_calibrated[i] && m_enabled[i])
		{
			// compute trajectory (new setpoint) only for enabled joints.
			switch (m_lastcommand[i])
			{
			case COMMAND_NONE:
			case COMMAND_SETPOS:
			default:
				break;

			case COMMAND_MOVETO:
				{
					m_intrajectory[i] = !m_trapezoidal.Step(i, &m_command[i]);

					// check limits (GetSpeedSetpoint gives the direction of motion).
					if (m_command[i] >= m_limits_max[i] && m_trapezoidal.GetSpeedSetpoint(i) > 0)
					{
						m_command[i] = m_limits_max[i];
						m_2b_speed[i] = 0.0;
						m_intrajectory[i] = false;
						//m_lastcommand[i] = COMMAND_NONE;
						m_trapezoidal.ResetTrajectory (i);
					}
					else
					if (m_command[i] <= m_limits_min[i] && m_trapezoidal.GetSpeedSetpoint(i) < 0)
					{
						m_command[i] = m_limits_min[i];
						m_2b_speed[i] = 0.0;
						m_intrajectory[i] = false;
						//m_lastcommand[i] = COMMAND_NONE;
						m_trapezoidal.ResetTrajectory (i);
					}

					if (m_intrajectory[i] == false)
					{
						m_lastcommand[i] = COMMAND_NONE;
					}
				}
				break;

			case COMMAND_VEL2:
				{
					double x;
					m_intrajectory[i] = !m_constvel.Step(i, &x);
					m_command[i] = x;

					// check limits.
					if (m_command[i] >= m_limits_max[i] && m_constvel.GetSpeedSetpoint(i) > 0)
					{
						m_command[i] = m_limits_max[i];
						m_2b_speed[i] = 0.0;
						m_intrajectory[i] = false;
						m_lastcommand[i] = COMMAND_NONE;
						m_constvel.ResetTrajectory (i);
					}
					else
					if (m_command[i] <= m_limits_min[i] && m_constvel.GetSpeedSetpoint(i) < 0)
					{
						m_command[i] = m_limits_min[i];
						m_2b_speed[i] = 0.0;
						m_intrajectory[i] = false;
						m_lastcommand[i] = COMMAND_NONE;
						m_constvel.ResetTrajectory (i);
					}

					if (m_intrajectory[i] == false)
					{
						m_lastcommand[i] = COMMAND_NONE;
					}
				}
				break;

			case COMMAND_VEL:
				if (m_intrajectory[i])
					m_command[i] += (m_speed[i] * THREAD_PERIOD);

				// check limits.
				if (m_command[i] >= m_limits_max[i] && m_speed[i] > 0)
				{
					m_command[i] = m_limits_max[i];
					m_2b_speed[i] = 0.0;
					m_intrajectory[i] = false;
					m_lastcommand[i] = COMMAND_NONE;
				}
				else
				if (m_command[i] <= m_limits_min[i] && m_speed[i] < 0)
				{
					m_command[i] = m_limits_min[i];
					m_2b_speed[i] = 0.0;
					m_intrajectory[i] = false;
					m_lastcommand[i] = COMMAND_NONE;
				}
				break;
			}
		}	/* if properly calibrated */
	}	/* loop over joints */
}

inline int _sgn(double x) { return (x>0) ? 1 : -1; }

// takes into account the torso (encoder) and the arms (pots).
// reads 3 joints from the MEI and the rest from the ueid.
// when calling this method, the m_position_zero must be already set.
// result in m_angle.
inline void __positioncontrol::ReadPositions (YARPArmControl& control)
{
	int stat = ueid_get_samples(ueidx, BOARD, MAX_SAMP, u_pot_vals);

	// torso.
	for (int i = 0; i < 3; i++)
	{
		int meidx = control.meidx0;
		int joint = control.get_mei_info (i, &meidx);
		m_angle[i] = double(meid_dsp_encoder (meidx, joint));

		// takes into account the 16bit wrapping.
		if (_sgn(m_angle[i]) < 0 && m_16bit_oldangle[i] > 16384.0 && m_winding[i] == 0)
		{
			m_winding[i] = 1;
		}
		else
		if (_sgn(m_angle[i]) > 0 && m_16bit_oldangle[i] < -16384.0 && m_winding[i] == 1)
		{
			m_winding[i] = 0;
		}
		else
		if (_sgn(m_angle[i]) > 0 && m_16bit_oldangle[i] < -16384.0 && m_winding[i] == 0)
		{
			m_winding[i] = -1;
		}
		else
		if (_sgn(m_angle[i]) < 0 && m_16bit_oldangle[i] > 16384.0 && m_winding[i] == -1)
		{
			m_winding[i] = 0;
		}

		m_16bit_oldangle[i] = int16(m_angle[i]);

		switch (m_winding[i])
		{
			case 1:
				m_angle[i] = 65535.0 + m_angle[i] - m_pot_zero[i];
				break;

			case -1:
				m_angle[i] = -65535.0 + m_angle[i] - m_pot_zero[i];
				break;

			case 0:
				m_angle[i] -= m_pot_zero[i];
				break;
		}
	}
	
	// the rest.
	for (i = 3; i < NJOINTS; i++)
	{
		m_angle[i] = 
			double(u_pot_vals[control.get_uei_info(i)] / __ueid_quantum);
	}
}

//
// class YARPArmControl.
//

// actual values need to be checked.
inline int YARPArmControl::get_uei_info (__jointtypes axis) const
{
	int ret = (int)axis;
	switch (ret) 
	{
		case TROLL:
		case TPITCH:
		case TYAW:
			return -1;	// these are not valid.
		break;
	
		case LSHOULDERA:
		case LSHOULDERB:
		case LELBOWA:
		case LELBOWB:
		case LWRISTA:
		case LWRISTB:
			return ret - 3;
		break;

		case LPAW:
		case LTHUMB:
			return ret + 3;
		break;

		case RSHOULDERA:
		case RSHOULDERB:
		case RELBOWA:
		case RELBOWB:
		case RWRISTA:
		case RWRISTB:
			return ret - 5;
		break;

		case RPAW:
		case RTHUMB:
			return ret - 3;
		break;

		default:
			fprintf(stderr, "illegal joint: %d\n", axis);
			return -1;
		break;
	}
}

// axis values should be ok now.
inline int YARPArmControl::get_mei_info(__jointtypes axis, int * meidx) const
{
	int ret = (int)axis;
	switch (ret) 
	{
		case TROLL:
			*meidx = meidx2;
			ret = 2;
			break;

		case TPITCH:
			*meidx = meidx2;
			ret = 0;
			break;

		case TYAW:
			*meidx = meidx2;
			ret = 1;
			break;

		case LSHOULDERA:
		case LSHOULDERB:
		case LELBOWA:
		case LELBOWB:
			*meidx = meidx0;
			ret = ret - 3;
			break;

		case LWRISTA:
		case LWRISTB:
			*meidx = meidx1;
			ret = ret - 7;
			break;

		case LPAW:
		case LTHUMB:
			*meidx = meidx2;
			ret = ret - 6;
			break;

		case RSHOULDERA:
		case RSHOULDERB:
		case RELBOWA:
		case RELBOWB:
			*meidx = meidx0;
			ret = ret - 7;
			break;

		case RWRISTA:
		case RWRISTB:
			*meidx = meidx1;
			ret = ret - 13;
			break;

		case RTHUMB:
		case RPAW:
			*meidx = meidx2;
			ret = ret - 12;
			break;

		default:
			fprintf(stderr, "illegal joint: %d\n", axis);
			break;
	}

	return ret;
}

inline void YARPArmControl::ReadZeroPositions (void)
{
	const int BUFFER = 10;

	// reads the zero from the potentiometers.
	// I read all 64 channels.
	uint16_t u_pot_vals[MAX_SAMP];
	for (int k = 0; k < BUFFER; k++)
	{
		int stat = ueid_get_samples(ueidx, BOARD, MAX_SAMP, u_pot_vals);
		delay(10);
	}

	for (int i = 3; i < NJOINTS; i++)
	{
		m_pot_zero[i] = u_pot_vals[get_uei_info(i)] / __ueid_quantum;
	}

	// takes care of the torso.
	for (i = 0; i < 3; i++)
	{
		int meidx = meidx0;
		int joint = get_mei_info (i, &meidx);
		// supposedly reset the encoder (not the DSP though).
		meid_set_position (meidx, joint, 0.0);
		m_pot_zero[i] = double(meid_dsp_encoder (meidx, joint));
	}
}

