//
// classes.inl.
//
//
//

//
//
//
//
//
// MODE 1 state machine. IDLE.
//
//
//
//

// state 0
template <class CALLER>
void IdleMode<CALLER>::ReachNull (CALLER *caller) {}

// state 1.
template <class CALLER>
void IdleMode<CALLER>::ReachMixedMessage (CALLER *caller)
{	
	for (int i = 0; i < m_nhmsg; i++)
	{
		caller->m_ohead.Content() = m_hmsg[i];
		caller->m_ohead.Write (1);
	}

	for (i = 0; i < m_namsg; i++)
		if (m_amsg[i].type != 0)
		{
			caller->m_oarm.Content() = m_amsg[i];
			caller->m_oarm.Write (1);
		}

	SetState (&IdleMode::ReachNull);
}

// state 2.
template <class CALLER>
void IdleMode<CALLER>::ReachHeadMessage (CALLER *caller)
{
	caller->m_ohead.Content() = m_hmsg[0];
	caller->m_ohead.Write (1);

	SetState (&IdleMode::ReachNull);
}

// state 3
template <class CALLER>
void IdleMode<CALLER>::ReachArmMessage (CALLER *caller)
{
	if (m_amsg[0].type != 0)
	{
		caller->m_oarm.Content() = m_amsg[0];
		caller->m_oarm.Write (1);
	}

	SetState (&IdleMode::ReachNull);
}

template <class CALLER>
IdleMode<CALLER>::IdleMode () : SmallStateMachine (4)
{
	m_array[0] = &IdleMode::ReachNull;
	m_array[1] = &IdleMode::ReachMixedMessage;
	m_array[2] = &IdleMode::ReachHeadMessage;
	m_array[3] = &IdleMode::ReachArmMessage;

	ResetState ();

	// init data.
	memset (m_hmsg, 0, sizeof(HeadMessage) * MAXNUMMESSAGES);
	memset (m_amsg, 0, sizeof(ArmMessage) * MAXNUMMESSAGES);
	m_namsg = 1;
	m_nhmsg = 1;
}

template <class CALLER>
void IdleMode<CALLER>::ResetState (void) 
{ 
	SetState (&IdleMode::ReachNull); 
}

template <class CALLER>
void IdleMode<CALLER>::SendArmMessage (ArmMessage *msg)
{
	Lock();
	memcpy (&m_amsg[0], msg, sizeof(ArmMessage));
	SetState (&IdleMode::ReachArmMessage);
	Unlock();
}

template <class CALLER>
void IdleMode<CALLER>::SendHeadMessage (HeadMessage *msg)
{
	Lock ();
	memcpy (&m_hmsg[0], msg, sizeof(HeadMessage));
	SetState (&IdleMode::ReachHeadMessage);
	Unlock ();
}

template <class CALLER>
void IdleMode<CALLER>::SendMessageSequence (ArmMessage *amsg, int namsg, HeadMessage *hmsg, int nhmsg)
{
	if (namsg >= MAXNUMMESSAGES || nhmsg >= MAXNUMMESSAGES)
		return;

	Lock ();
	memcpy (m_hmsg, hmsg, sizeof(HeadMessage) * nhmsg);
	memcpy (m_amsg, amsg, sizeof(ArmMessage) * namsg);
	m_nhmsg = nhmsg;
	m_namsg = namsg;
	SetState (&IdleMode::ReachMixedMessage);
	Unlock ();
}


//
//
//
//
//
// Mode 2
// Standard reach mode (sort of general purpose).
//
//
//
//
template <class CALLER>
void StdReachingMode<CALLER>::ForwardKin (CVisDVector& out) 
{
	// combine and return in out vector.
	m_gaze.Apply (m_pos);

	out(1) = m_gaze.theta_middle * radToDeg;
	out(2) = m_gaze.phi_middle * radToDeg;
}

template <class CALLER>
void StdReachingMode<CALLER>::Common (CALLER *caller)
{
	caller->m_headpos.Read();
	caller->m_armpos.Read();

	m_positionarm = caller->m_armpos.Content();

	m_now = clock();
	m_last_cycle = ((m_now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC);
	m_prev_cycle = m_now;

	m_pos = caller->m_headpos.Content();
	for (int i = 1; i <= 7; i++)
		m_speed(i) = (m_pos(i) - m_old_pos(i)) / m_last_cycle;
	m_old_pos = m_pos;

	m_count_fatique ++;
	if (m_count_fatique > 300)
	{
		SetState (&StdReachingMode::ReachRelax);
	}
}

// state 0.
template <class CALLER>
void StdReachingMode<CALLER>::ReachNull (CALLER *) 
{
}

// state 1
template <class CALLER>
void StdReachingMode<CALLER>::ReachIdle (CALLER *)
{
	// init the sequence...
	// if there's something to reach.
	double vergence = m_pos.j2 - m_pos.j3;
	double version = .5 * (m_pos.j3 + m_pos.j2);

	bool moving = false;
	for (int i = 1; i <= 7; i++)
		moving |= (fabs(m_speed(i)) > 6000);

	m_count_reaching--;

	if (vergence > .1 * 20000 && 
		vergence < 0.98 * 20000 &&
		!moving &&
		m_count_reaching <= 0)
	{
		// there's something in front of the robot.
		// try a reaching movement.
		printf ("object present?\n");
		SetState (&StdReachingMode::ReachInit);
	}
}

// state 2
template <class CALLER>
void StdReachingMode<CALLER>::ReachInit (CALLER *caller)
{
	// Query the LUT.
	ForwardKin (m_curg);

	m_tin[0] = m_curg(1); //m_pos.j6;
	m_tin[1] = m_curg(2); //m_pos.j7;

	printf ("curg: %lf %lf\n", m_curg(1), m_curg(2));

	// these are the workspace limit... temporary.
	if (m_tin[1] < 30.0 || m_tin[0] > 10.0)
	{
		printf ("outside workspace...\n");
		m_count_reaching = -1;		// allow immediate new reach.
		m_count_fatique = 0;

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgMoveToOld;
		for (int i = 0; i < 6; i++)
		{
			m_amsg.j[i+3] = primitives[0][i];
		}
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		SetState (&StdReachingMode::ReachIdle);
	}
	else
	{
		m_lut.Nearest (2, m_tin, NPRIMITIVES, m_tout);

		memset (m_tjnt, 0, sizeof(double) * 6);
		for (int i = 0; i < NPRIMITIVES; i++)
		{
			for (int j = 0; j < 6; j++)
				m_tjnt[j] += (m_tout[i] * primitives[i][j]); 
		}

		printf ("LUT response\n");
		for (i = 0; i < 6; i++)
		{
			printf ("%f ", m_tjnt[i]);
		}
		printf ("\n");

		for (i = 0; i <= 4; i++)
		{
			m_tjnt[i] += (YARPRandom::Rand (-m_noiselevel,m_noiselevel));
		}

		printf ("Noisy response\n");
		for (i = 0; i < 6; i++)
		{
			printf ("%f ", m_tjnt[i]);
		}
		printf ("\n");

		m_count_reaching = REACHLENGTH;

		// if I don't know where the hand/arm is...
		// go into the sequence to generate appropriate
		// movements for localization.
		// it needs to wait for reaching completion, to stop
		// the head, move the arm for a while...
		// if located recover the hand position from a tracker
		// and eventually go intoa closed loop control mode?
		SetState (&StdReachingMode::ReachMove);

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgMoveToOld;
		for (i = 3; i <= 8; i++)
			m_amsg.j[i] = m_tjnt[i-3];

		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);
	}
}


// state 3
template <class CALLER>
void StdReachingMode<CALLER>::ReachMove (CALLER *caller)
{
	m_count_reaching--;
	if (m_count_reaching < 0)
	{
		if (m_positionarm.j[5] < 315.0 ||
			m_positionarm.j[6] < 524.0 ||
			m_positionarm.j[3] < 344.0 ||
			m_tjnt[2] < 315.0 ||
			m_tjnt[3] < 524.0 ||
			m_tjnt[0] < 344.0)
		{
			// too close to wave the arm.
			SetState (&StdReachingMode::ReachIdle);
		}
		else
		{
			if (m_wave)
				SetState (&StdReachingMode::ReachWave);
			else
				SetState (&StdReachingMode::ReachIdle);
		}
	}
}


// state 4
template <class CALLER>
void StdReachingMode<CALLER>::ReachWave (CALLER *caller)
{
	m_count_reaching = REACHLENGTH;
	SetState (&StdReachingMode::ReachIdle);

	// stop the head and torso.
	memset (&m_hmsg, 0, sizeof(HeadMessage));
	m_hmsg.type = HeadMsgStopAll;
	caller->m_ohead.Content() = m_hmsg;
	caller->m_ohead.Write (1);

	printf ("Head stop message sent\n");

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgTorsoStop;
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	for (int i = 0; i < 2; i++)
	{
		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgWaveHand;
		m_amsg.j[3] = 0;
		m_amsg.j[4] = 0;
		m_amsg.j[5] = -300;
		m_amsg.j[6] = -500;
		m_amsg.j[7] = -800;
		m_amsg.j[8] = 0;
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		// the logic here should be a little smarter.
		YARPTime::DelayInSeconds (2.5);
	}

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgTorsoStart;
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	// restart head and torso motion.
	memset (&m_hmsg, 0, sizeof(HeadMessage));
	m_hmsg.type = 0;
	caller->m_ohead.Content() = m_hmsg;
	caller->m_ohead.Write (1);

	printf ("Head reset message sent\n");

	m_count_fatique += 30;
	// don't move the arm for a moment.	
	// see whether it helps the tracker.

	YARPTime::DelayInSeconds (2.0);

	m_count_fatique += 30;
}

// state 5
template <class CALLER>
void StdReachingMode<CALLER>::ReachRelax (CALLER *caller)
{
	m_count_reaching = RELAXLENGTH;
	m_count_fatique = 0;

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 0; i < 6; i++)
		m_amsg.j[i+3] = primitives[0][i];
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	SetState (&StdReachingMode::ReachIdle);
}


template <class CALLER>
StdReachingMode<CALLER>::StdReachingMode () : SmallStateMachine (6)
{
	m_array[0] = &StdReachingMode::ReachNull;
	m_array[1] = &StdReachingMode::ReachIdle;
	m_array[2] = &StdReachingMode::ReachInit;
	m_array[3] = &StdReachingMode::ReachMove;
	m_array[4] = &StdReachingMode::ReachWave;
	m_array[5] = &StdReachingMode::ReachRelax;

	ResetState ();

	// Initialize data.
	memset (&m_amsg, 0, sizeof(ArmMessage));
	memset (&m_hmsg, 0, sizeof(HeadMessage));
	
	m_lut.Init (2, NPRIMITIVES, 100);

	// current gaze vector.
	m_curg.Resize (2);
	m_curg = 0;

	// add primitives to the map.
	for (int i = 0; i < NPRIMITIVES; i++)
		m_lut.AddNew (2, (double *)input_prim[i], NPRIMITIVES, (double *)output_prim[i]);

	// add a fourth point in between.
	m_tin[0] = -15.0;
	m_tin[1] = 60.0;
	m_tout[0] = 0.0;
	m_tout[1] = 0.5;
	m_tout[2] = 0.5;
	m_lut.AddNew (2, m_tin, NPRIMITIVES, m_tout);

	m_prev_cycle = clock();

	m_count_reaching = 0;
	m_count_fatique = 0;

	memset (&m_old_pos, 0, sizeof(JointPos));
	memset (&m_speed, 0, sizeof(JointPos));

	m_wave = false;
	m_noiselevel = 0;
	YARPRandom::Seed (time(NULL));
}

template <class CALLER>
void StdReachingMode<CALLER>::ResetState (void) 
{ 
	SetState (&StdReachingMode::ReachIdle); 
}

template <class CALLER>
void StdReachingMode<CALLER>::ToggleWaveArm (void)
{
	Lock ();
	m_wave = !m_wave; 
	Unlock ();
}

template <class CALLER>
void StdReachingMode<CALLER>::SetNoiseLevel (double noise)
{
	Lock ();
	m_noiselevel = noise; 
	Unlock ();
}


//
//
//
//
//
// Mode 3.
// reaching 2D on a table, open loop.
//
//
//
//

// must read also from the inertial sensor!
template <class CALLER>
void PlanarMode<CALLER>::Common (CALLER *caller)
{
	caller->m_headpos.Read();
	caller->m_armpos.Read();
	caller->m_targetpos.Read();

	m_positionarm = caller->m_armpos.Content();
	m_image_target = caller->m_targetpos.Content();

	m_now = clock();
	m_last_cycle = ((m_now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC);
	m_prev_cycle = m_now;

	m_pos = caller->m_headpos.Content();
	for (int i = 1; i <= 7; i++)
		m_speed(i) = (m_pos(i) - m_old_pos(i)) / m_last_cycle;
	m_old_pos = m_pos;

	// LATER:
	//	it should not interrupt reaching.
	m_count_fatique ++;
	m_signal_counter ++;

	if (
		m_count_fatique > FATIQUE && m_calibrated && 
		(GetState() == &PlanarMode::ReachIdle ||
		 GetState() == &PlanarMode::ReachNull)
	   )
	{
		SetState (&PlanarMode::ReachWait);
		m_count_reaching = RELAXLENGTH;
		m_count_sidepos = 0;
		m_count_fatique = 0;
	}
}

// state 0 no calibration.
template <class CALLER>
void PlanarMode<CALLER>::ReachNull (CALLER *) 
{
}

template <class CALLER>
bool PlanarMode<CALLER>::NotMovingHead (void)
{
	bool moving = false;
	const double thr = 5;
	const int length = 20;

	moving |= (fabs(m_speed(1)) > thr);
	moving |= (fabs(m_speed(2)) > thr);
	moving |= (fabs(m_speed(3)) > thr);
	moving |= (fabs(m_speed(6)) > thr);
	moving |= (fabs(m_speed(7)) > thr);

	if (!moving)
		m_moving_counter++;
	else
		m_moving_counter = 0;

	if (m_moving_counter > length)
	{
		m_moving_counter = 0;
		printf ("Head is not moving\n");
		return true;
	}
	
	return false;
}

// state 1
template <class CALLER>
void PlanarMode<CALLER>::ReachIdle (CALLER *caller)
{
	if (!m_calibrated)
	{
		SetState (&PlanarMode::ReachNull);
		return;
	}
	
	// given the joint position returns a vector of size INSIZE (fixation point?).
	m_reaching.ForwardKinHead (m_pos, m_curg);

	// BTW: 43500 is the conversion factor between encoders and angles!!!!
	double vergence = (m_pos.j2 - m_pos.j3) * pi / (2 * 43500);
	double v_expected = 0;

	m_reaching.ComputeVergence (m_curg, v_expected);

	m_count_reaching--;

	double c1, c2;
	m_reaching.ComputeFixationPoint (m_curg, c1, c2);

	bool moving = NotMovingHead();

	const double dx = m_image_target.xr - Size/2;
	const double dy = m_image_target.yr - Size/2;
	double image_error = sqrt(dx*dx+dy*dy);

	//
	//
	//
	if (m_signal_counter > 20)
	{
		m_signal_counter = 0;
		m_signal = true;
	}

	const double RANGE = 1.5;
	// if it's looking roughly towards the table... signal the condition.
	if (c1 >= -1.0-RANGE && c1 <= 0.5+RANGE && c2 > -6.6-RANGE && c2 < -5.3+RANGE && m_signal == true)
	{
		caller->m_msg_port.SendMsg (YBC_REACH_RANGE);
		m_signal = false;
	}

	if (!(c1 >= -1.0-RANGE && c1 <= 0.5+RANGE && c2 > -6.6-RANGE && c2 < -5.3+RANGE) && m_signal == true)
	{
		caller->m_msg_port.SendMsg (YBC_REACH_OUT_OF_RANGE);
		m_signal = false;
	}
	//
	//
	//


	// there is a bunch of thresholds here.
	if (
		moving &&
		m_count_reaching <= 0 &&
		(c1 >= -1.0 && c1 <= 0.5 && c2 > -6.6 && c2 < -5.3) &&
		fabs (vergence - v_expected) < 0.07	&& /* it was 0.05 */
		image_error < 10
		)
	{
		// there's something in front of the robot.
		// gaze is almost on the plane.
		// try a reaching movement.

		printf ("object within the workspace, start movement\n");

		// store last known target position.
		m_store_image_target = m_image_target;

		// stop the head/torso motion.
		memset (&m_hmsg, 0, sizeof(HeadMessage));
		m_hmsg.type = HeadMsgStopAll;
		caller->m_ohead.Content() = m_hmsg;
		caller->m_ohead.Write (1);

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgTorsoStop;
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		// communicate to the higher level control.
		caller->m_msg_port.SendMsg (YBC_SHLC_START_REACHING);

		m_count_sidepos = 0;
		SetState (&PlanarMode::ReachSidepositionPrepare);

		m_signal = true;
	}
}

// LIMITS... determined by hand.
template <class CALLER>
void PlanarMode<CALLER>::ClipControl (CVisDVector& c)
{
	if (c(1) < 244) c(1) = 244;
	if (c(1) > 444) c(1) = 444;
	if (c(2) < 240) c(2) = 240;
	if (c(2) > 290) c(2) = 290;
	if (c(3) < 195) c(3) = 195;
	if (c(3) > 415) c(3) = 415;
	if (c(4) < 454) c(4) = 454;
	if (c(4) > 704) c(4) = 704;
}

// state 2
template <class CALLER>
void PlanarMode<CALLER>::ReachInit (CALLER *caller)
{
	m_reaching.ComputeReaching (m_pos, m_initial_position, m_control);

	m_count_reaching = REACHLENGTH;

	// move assuming the previous position was already with the elbow flexed.
	SetState (&PlanarMode::ReachMove);

	// 
	//	for backslap
	if (m_initial_position == 0)
		caller->m_msg_port.SendMsg (YBC_SHLC_START_PUSHING, m_initial_position);

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 3; i <= 8; i++)
		m_amsg.j[i] = m_control(i-2);

	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);
}


// state 12
template <class CALLER>
void PlanarMode<CALLER>::ReachInit2 (CALLER *caller)
{
	const double NOISE = 10.0;	// degrees on gaze.
	m_reaching.ComputeReaching (m_pos, -1, m_control, NOISE * degToRad);
	m_control (4) -= 40;

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 3; i <= 8; i++)
		m_amsg.j[i] = m_control(i-2);

	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	m_count_reaching = REACHLENGTH;

	SetState (&PlanarMode::ReachMove2);
}

// state 13
template <class CALLER>
void PlanarMode<CALLER>::ReachMove2 (CALLER *caller)
{
	m_count_reaching--;

	if (m_count_reaching >= 0)
	{
		printf ("MOVE2\n");

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgMoveToOld;
		for (int i = 3; i <= 8; i++)
			m_amsg.j[i] = m_control(i-2);

		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		YARPTime::DelayInSeconds (0.2);
	}
	else
	{
		printf ("SHAKING\n");

		for (int i = 0; i < 2; i++)
		{
			memset (&m_amsg, 0, sizeof(ArmMessage));
			m_amsg.type = ArmMsgWaveHand;
			m_amsg.j[3] = 0;
			m_amsg.j[4] = 0;
			m_amsg.j[5] = 0;
			m_amsg.j[6] = 0;
			m_amsg.j[7] = -800;
			m_amsg.j[8] = 0;
			caller->m_oarm.Content() = m_amsg;
			caller->m_oarm.Write (1);

			// the logic here should be a little smarter.
			YARPTime::DelayInSeconds (2.5);
		}

		//
		// communicate to the higher level control.
		caller->m_msg_port.SendMsg (YBC_SHLC_STOP_REACHING);

		// restart head motion.
		memset (&m_hmsg, 0, sizeof(HeadMessage));
		m_hmsg.type = 0;
		caller->m_ohead.Content() = m_hmsg;
		caller->m_ohead.Write (1);

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgTorsoStart;
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgTorsoLink2Head;
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		SetState (&PlanarMode::ReachWait);
		m_count_reaching = RELAXLENGTH;
		m_count_sidepos = SIDEPOS-1;
		m_count_fatique = 0;

		YARPTime::DelayInSeconds (1.0);
	}
}

// state 3
template <class CALLER>
void PlanarMode<CALLER>::ReachMove (CALLER *caller)
{
	m_count_reaching--;

	if (m_count_reaching >= 0)
	{
		printf ("MOVE\n");

		// LATER: if mode == 0 this is the pushing...

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgMoveToOld;
		for (int i = 3; i <= 8; i++)
			m_amsg.j[i] = m_control(i-2);

		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		YARPTime::DelayInSeconds (0.2);
	}
	else
	{
		printf ("PUSHING\n");

		//
		// LATER: need to be improved.
		// perhaps move to the programmed reaching position (w/ correct gaze).
		switch (m_initial_position)
		{
		case 1:
		case 2:
		case 3:
			{
				// communicate to the higher level control.
				caller->m_msg_port.SendMsg (YBC_SHLC_START_PUSHING, m_initial_position);

				memset (&m_amsg, 0, sizeof(ArmMessage));
				m_amsg.type = ArmMsgPushOne;
				for (int j = 1; j <= 6; j++)
					m_amsg.j[j+2] = m_reaching.GetPushingValue(m_initial_position)(j);

				caller->m_oarm.Content() = m_amsg;
				caller->m_oarm.Write (1);
				
				YARPTime::DelayInSeconds (1.0);

				// go to side position.
				memset (&m_amsg, 0, sizeof(ArmMessage));
				m_amsg.type = ArmMsgMoveToOld;
				for (int i = 3; i <= 8; i++)
					m_amsg.j[i] = m_reaching.GetSideposition()(i-2);

				caller->m_oarm.Content() = m_amsg;
				caller->m_oarm.Write (1);

				YARPTime::DelayInSeconds (1.0);

				// communicate to the higher level control.
				caller->m_msg_port.SendMsg (YBC_SHLC_STOP_PUSHING);

				YARPTime::DelayInSeconds (0.2);
			}
			break;

		case 0:
			{
				caller->m_msg_port.SendMsg (YBC_SHLC_STOP_PUSHING);

				memset (&m_amsg, 0, sizeof(ArmMessage));
				m_amsg.type = ArmMsgMoveToOld;
				for (int i = 3; i <= 8; i++)
					m_amsg.j[i] = m_reaching.GetSideposition()(i-2);

				caller->m_oarm.Content() = m_amsg;
				caller->m_oarm.Write (1);

				YARPTime::DelayInSeconds (1.0);
			}
			break;
		}

		//
		// communicate to the higher level control.
		caller->m_msg_port.SendMsg (YBC_SHLC_STOP_REACHING);

		// restart head motion.
		memset (&m_hmsg, 0, sizeof(HeadMessage));
		m_hmsg.type = 0;
		caller->m_ohead.Content() = m_hmsg;
		caller->m_ohead.Write (1);

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgTorsoStart;
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		memset (&m_amsg, 0, sizeof(ArmMessage));
		m_amsg.type = ArmMsgTorsoLink2Head;
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);

		SetState (&PlanarMode::ReachWait);
		m_count_reaching = RELAXLENGTH;
		m_count_sidepos = SIDEPOS-1;
		m_count_fatique = 0;

		YARPTime::DelayInSeconds (1.0);
	}
}

// state 4
template <class CALLER>
void PlanarMode<CALLER>::ReachRelax (CALLER *caller)
{
	printf ("RELAX\n");

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 0; i < 6; i++)
		m_amsg.j[i+3] = primitives_planar[0][i];
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	// restart head motion.
	memset (&m_hmsg, 0, sizeof(HeadMessage));
	m_hmsg.type = 0;
	caller->m_ohead.Content() = m_hmsg;
	caller->m_ohead.Write (1);

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgTorsoStart;
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	m_count_reaching--;

	if (m_count_reaching < 0)
	{
		m_count_fatique = 0;
		if (m_calibrated)
		{
			SetState (&PlanarMode::ReachIdle);
			m_count_reaching = 0;
		}
		else
			SetState (&PlanarMode::ReachNull);
	}
}

// state 5
template <class CALLER>
void PlanarMode<CALLER>::ReachHeadMessage (CALLER *caller)
{
	printf ("HEAD MSG\n");

	caller->m_ohead.Content() = m_hmsg;
	caller->m_ohead.Write (1);

	SetState (m_storestate);
}

// state 6
template <class CALLER>
void PlanarMode<CALLER>::ReachArmMessage (CALLER *caller)
{
	printf ("ARM MSG\n");

	if (m_amsg.type != 0)
	{
		caller->m_oarm.Content() = m_amsg;
		caller->m_oarm.Write (1);
		m_last_commanded = true;

		for (int i = 1; i <= ARMSIZE; i++)
			m_commanded(i) = m_amsg.j[i+2];
	}

	SetState (m_storestate);
}

// state 10
template <class CALLER>
void PlanarMode<CALLER>::ReachSidepositionPrepare (CALLER *caller)
{
	printf ("SIDEPOSITION PREPARE\n");

	int i;

	// go to side position.
	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (i = 3; i <= 8; i++)
		m_amsg.j[i] = m_reaching.GetSideposition()(i-2);

	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	memset (&m_hmsg, 0, sizeof(HeadMessage));
	m_hmsg.type = HeadMsgStopAll;
	caller->m_ohead.Content() = m_hmsg;
	caller->m_ohead.Write (1);

	m_torso = 0;

	// first of all, wait for a message to start reaching/poking sequence.
	// - if (roughly) there's no movement then start poking.
	//	otherwise, just observe...
	
	// IMAGE_MOTION.
	double starttime = YARPTime::GetTimeAsSeconds();
	double curtime = starttime;
	bool call_timeout = false;
	int counter = 0;
	
	do
	{
		call_timeout = (caller->m_ans_port.GetMsgBlock (YBC_IMAGE_MOTION, 1.0) == -1) ? true : false;
		if (!call_timeout) counter ++;

		printf ("call_timeout: %d, counter: %d\n", call_timeout, counter);
		curtime = YARPTime::GetTimeAsSeconds();
	}
	while (curtime - starttime < 1.3);

	// TEST. override timeout to test behavior.
	//call_timeout = false;

	// if last 100ms out of 500ms there was no motion, then reach/poke.
	//if (call_timeout)
	if (counter <= 5)
	{
		printf ("reaching Ok, there was no movement in the last xxx ms\n");

		// ok for reaching.
		// get decision on action to take from shlc.
		printf ("about to send a request to shlc for best action to take\n");
		caller->m_msg_port.SendMsg (YBC_SHLC_COMPUTE_BEST_ACTION);

		// wait for reply (300ms timeout).
		caller->m_ans_port.GetMsgBlock (YBC_SHLC_COMPUTE_BEST_ACTION_REPLY, m_action_no);

		printf ("received a reply from shlc, action: %d\n", m_action_no);

		SetState (&PlanarMode::ReachSideposition);

		m_count_sidepos ++;
	}
	else
	{
		// new state for recording.
		printf ("No reaching, too much movement in the scene\n");

		SetState (&PlanarMode::RecordingOnly);
	}
}


// state 11
template <class CALLER>
void PlanarMode<CALLER>::RecordingOnly (CALLER *caller)
{
	// inform shlc that this has to be processed.
	caller->m_msg_port.SendMsg (YBC_SHLC_START_RECORDING);

	// move the arm back to relax.
	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 0; i < 6; i++)
		m_amsg.j[i+3] = primitives_planar[0][i];
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	// just wait...
	YARPTime::DelayInSeconds (1.8);
	caller->m_msg_port.SendMsg (YBC_SHLC_STOP_RECORDING);

	//
	// communicate to the higher level control.
	YARPTime::DelayInSeconds (0.2);
	caller->m_msg_port.SendMsg (YBC_SHLC_STOP_REACHING);

	// restart head motion.
	memset (&m_hmsg, 0, sizeof(HeadMessage));
	m_hmsg.type = 0;
	caller->m_ohead.Content() = m_hmsg;
	caller->m_ohead.Write (1);

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgTorsoStart;
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgTorsoLink2Head;
	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	SetState (&PlanarMode::ReachWait);
	m_count_reaching = RELAXLENGTH;
	m_count_sidepos = SIDEPOS-1;
	m_count_fatique = 0;
}


// state 7
template <class CALLER>
void PlanarMode<CALLER>::ReachSideposition (CALLER *caller)
{
	// go to side position.
	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 3; i <= 8; i++)
		m_amsg.j[i] = m_reaching.GetSideposition()(i-2);

	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	YARPTime::DelayInSeconds (0.2);
	m_count_sidepos++;

	if (m_wave_arm)
	{
		// in this case follow a different sequence to shake the arm.
		SetState (&PlanarMode::ReachInit2);
	}
	else
	{
		// negative action means failure.
		if (m_action_no >= 0)
		{
			if (m_count_sidepos > SIDEPOS)
			{
				printf ("got a reply from higher level, poking\n");

				switch (m_action_no)
				{
				case 0:
						printf ("Backslap\n");
						SetState (&PlanarMode::ReachSideposition2);
						m_initial_position = 0;
					break;

				case 1:
						printf ("position 1\n");
						SetState (&PlanarMode::ReachInit);
						m_initial_position = 1;
					break;

				case 2:
						printf ("position 2\n");
						SetState (&PlanarMode::ReachInit);
						m_initial_position = 2;
					break;

				case 3:
						printf ("position 3\n");
						SetState (&PlanarMode::ReachInit);
						m_initial_position = 3;
					break;
				}
			}
		}
		else
		{
			// decide what action to take (random).
			if (m_count_sidepos > SIDEPOS)
			{
				printf ("Can't get info from higher level - random poking\n");

				int r = YARPRandom::Rand (0,3);
				printf ("r = %d\n", r);
				switch (r)
				{
				case 0:
						printf ("Backslap\n");
						SetState (&PlanarMode::ReachSideposition2);
						m_initial_position = 0;
					break;

				case 1:
						printf ("position 1\n");
						SetState (&PlanarMode::ReachInit);
						m_initial_position = 1;
					break;

				case 2:
						printf ("position 2\n");
						SetState (&PlanarMode::ReachInit);
						m_initial_position = 2;
					break;

				case 3:
						printf ("position 3\n");
						SetState (&PlanarMode::ReachInit);
						m_initial_position = 3;
					break;
				}
			}
		}
	}
}

// state 9
template <class CALLER>
void PlanarMode<CALLER>::ReachSideposition2 (CALLER *caller)
{
	// go to side position.
	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 3; i <= 8; i++)
		m_amsg.j[i] = chestposition[i-3];

	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	YARPTime::DelayInSeconds (2.0);

	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (i = 3; i <= 8; i++)
		m_amsg.j[i] = chestposition[i-3];
	m_amsg.j[6] += 190;
	m_amsg.j[5] -= 120;

	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	YARPTime::DelayInSeconds (2.0);

	// why do I need the if?
	//if (m_count_sidepos > SIDEPOS)
	//{
	SetState (&PlanarMode::ReachInit);
	//}
}

template <class CALLER>
bool PlanarMode<CALLER>::CloseEnough (void)
{
	bool ret = true;

	printf ("error :");
	for (int i = 0; i < 4; i++)
	{
		ret &= (fabs(m_positionarm.j[i+3]-primitives_planar[0][i]) < 50);
		printf ("%lf ",fabs(m_positionarm.j[i+3]-primitives_planar[0][i]));
	}
	printf ("\n");

	return ret;
}

// state 8
template <class CALLER>
void PlanarMode<CALLER>::ReachWait(CALLER *caller) 
{
	// Don't do this if already relax...
	if (CloseEnough())
	{
		SetState (&PlanarMode::ReachRelax);
		return;
	}

	// go to side position (before moving to relax).
	memset (&m_amsg, 0, sizeof(ArmMessage));
	m_amsg.type = ArmMsgMoveToOld;
	for (int i = 3; i <= 8; i++)
		m_amsg.j[i] = m_reaching.GetSideposition()(i-2);

	caller->m_oarm.Content() = m_amsg;
	caller->m_oarm.Write (1);

	YARPTime::DelayInSeconds (0.2);

	m_count_sidepos++;
	if (m_count_sidepos > SIDEPOS)
	{
		SetState (&PlanarMode::ReachRelax);
	}
}

template <class CALLER>
PlanarMode<CALLER>::PlanarMode () : SmallStateMachine (14), m_reaching (INSIZE, ARMSIZE, BASISV)
{
	m_array[0] = &PlanarMode::ReachNull;
	m_array[1] = &PlanarMode::ReachIdle;
	m_array[2] = &PlanarMode::ReachInit;
	m_array[3] = &PlanarMode::ReachMove;
	m_array[4] = &PlanarMode::ReachRelax;
	m_array[5] = &PlanarMode::ReachHeadMessage;
	m_array[6] = &PlanarMode::ReachArmMessage;
	m_array[7] = &PlanarMode::ReachSideposition;
	m_array[8] = &PlanarMode::ReachWait;
	m_array[9] = &PlanarMode::ReachSideposition2;
	m_array[10] = &PlanarMode::ReachSidepositionPrepare;
	m_array[11] = &PlanarMode::RecordingOnly;
	m_array[12] = &PlanarMode::ReachInit2;
	m_array[13] = &PlanarMode::ReachMove2;

	m_storestate = &PlanarMode::ReachNull;

	SetState (&PlanarMode::ReachNull);

	// Initialize data.
	memset (&m_amsg, 0, sizeof(ArmMessage));
	memset (&m_hmsg, 0, sizeof(HeadMessage));
	
	m_prev_cycle = clock();

	m_count_reaching = 0;
	m_count_fatique = 0;
	m_moving_counter = 0;
	m_signal_counter = 0;

	memset (&m_old_pos, 0, sizeof(JointPos));
	memset (&m_speed, 0, sizeof(JointPos));

	m_curg.Resize (INSIZE); m_curg = 0;

	m_control.Resize (ARMSIZE);
	m_control = 0;

	m_commanded.Resize (ARMSIZE);
	m_commanded = 0;

	m_last_commanded = false;
	m_calibrated = false;
	m_signal = true;
	m_wave_arm = false;

	m_initial_position = 0;

	m_fixx = m_fixy = 0;
	m_newfixationpoint = false;

	m_torso = 0;
	m_action_no = -1;
}

template <class CALLER>
void PlanarMode<CALLER>::ResetState (void) 
{ 
	SetState (&PlanarMode::ReachIdle); 
	m_last_commanded = false; 
}

template <class CALLER>
void PlanarMode<CALLER>::BuildTemporaryVectors (void)
{
	Lock ();
	m_reaching.BuildTemporaryVectors();
	m_calibrated = true;

	// go into a working state.
	SetState (&PlanarMode::ReachIdle);

	Unlock ();
}

template <class CALLER>
void PlanarMode<CALLER>::LoadCalibration (const char *filename) 
{
	Lock ();

	m_reaching.LoadCalibration (filename);
	m_calibrated = true;

	// go into a working state.
	SetState (&PlanarMode::ReachIdle);

	Unlock ();
}


template <class CALLER>
void PlanarMode<CALLER>::Uncalibrate (void)
{
	m_reaching.Uncalibrate ();

	m_calibrated = false;
	SetState (&PlanarMode::ReachNull);
}

template <class CALLER>
void PlanarMode<CALLER>::StoreCalibration (const char *filename)
{
	Lock ();
	m_reaching.StoreCalibration (filename);
	Unlock ();
}

// it stores the commanded position rather the actual.
template <class CALLER>
void PlanarMode<CALLER>::StoreSidePosition (void)
{
	if (!m_last_commanded)
		return;

	Lock ();
	m_reaching.SetSideposition (m_commanded);

	CVisDVector& sideposition = m_reaching.GetSideposition();

	// DEBUG
	printf ("Stored side position : ");
	for (int i = 1; i <= ARMSIZE; i++)
		printf ("%lf ", sideposition(i));
	printf ("\n");

	Unlock ();
}

template <class CALLER>
void PlanarMode<CALLER>::StoreBasisPosition (int ii)
{
	if (ii < 0 || ii >= BASISV || !m_last_commanded)
		return;

	Lock ();
	m_reaching.SetBasis (ii, m_pos, m_commanded);

	CVisDVector gaze (INSIZE);
	CVisDVector arm (ARMSIZE);
	double v;

	m_reaching.GetBasis (ii, gaze, arm, v);

	// DEBUG
	printf ("Stored basis %d\n", ii);
	printf ("Head : ");
	for (int i = 1; i <= INSIZE; i++)
		printf ("%lf ", gaze(i));
	printf ("\n");
	printf ("Arm : ");
	for (i = 1; i <= ARMSIZE; i++)
		printf ("%lf ", arm(i));
	printf ("\n");
	printf ("Vergence : %lf\n", v);

	Unlock ();
}

template <class CALLER>
void PlanarMode<CALLER>::SendArmMessage (ArmMessage *msg)
{
	Lock();
	m_storestate = GetState();
	memcpy (&m_amsg, msg, sizeof(ArmMessage));
	SetState (&PlanarMode::ReachArmMessage);
	Unlock();
}

template <class CALLER>
void PlanarMode<CALLER>::SendHeadMessage (HeadMessage *msg)
{
	Lock ();
	m_storestate = GetState();
	memcpy (&m_hmsg, msg, sizeof(HeadMessage));
	SetState (&PlanarMode::ReachHeadMessage);
	Unlock ();
}

template <class CALLER>
void PlanarMode<CALLER>::SetPreciseFixationPnt (int x, int y)
{
	Lock ();
	m_fixx = x;
	m_fixy = y;
	m_newfixationpoint = true;
	Unlock ();
}

