//
// HeadControl.inl
//

//
// Class YARPGenericControl
//

/* message-passing wrappers */
inline int16 YARPGenericControl::DspReset (void) { return meid_dsp_reset(meidx); }

inline int16 YARPGenericControl::StartMove (int16 axis, double position, double velocity, double acceleration)
{
	return meid_start_move(meidx, axis, position, velocity, acceleration);
}

inline int16 YARPGenericControl::StartMoveAll (int16 len, int16 *axes,
		  double *position, double *velocity,
		  double *acceleration)
{
	return meid_start_move_all(meidx, len, axes,
		  position, velocity,
		  acceleration);
}

inline int16 YARPGenericControl::GetVelocity (int16 axis, double *velocity)
{
	return meid_get_velocity(meidx, axis, velocity);
}

inline int16 YARPGenericControl::GetAccel (int16 axis, double *accel)
{
	return meid_get_accel(meidx, axis, accel);
}

inline int16 YARPGenericControl::GetJerk (int16 axis, double *jerk)
{
	return meid_get_jerk(meidx, axis, jerk);
}

inline int16 YARPGenericControl::GetCommand (int16 axis, double *command)
{
	return meid_get_command(meidx, axis, command);
}

inline int16 YARPGenericControl::GetPosition (int16 axis, double *position)
{
	return meid_get_position(meidx, axis, position);
}

inline int16 YARPGenericControl::GetError (int16 axis, double *error)
{
	return meid_get_error(meidx, axis, error);
}

inline int16 YARPGenericControl::GetFilter (int16 axis, int16 *coeffs)
{
	return meid_get_filter(meidx, axis, coeffs);
}

inline int16 YARPGenericControl::SetFilter (int16 axis, int16 *coeffs)
{
	return meid_set_filter(meidx, axis, coeffs);
}

inline int16 YARPGenericControl::GetInPosition (int16 axis, double *limit)
{
	return meid_get_in_position(meidx, axis, limit);
}

inline int16 YARPGenericControl::SetInPosition (int16 axis, double limit)
{
	return meid_set_in_position(meidx, axis, limit);
}

inline int16 YARPGenericControl::FramesLeft (int16 axis)
{
	return meid_frames_left(meidx, axis);
}

inline int16 YARPGenericControl::FramesToExecute (int16 axis)
{
	return meid_frames_to_execute(meidx, axis); /* int16, mebbe??? */
}

inline int16 YARPGenericControl::AxisStatus (int16 axis)
{
	return meid_axis_status(meidx, axis);
}

inline int16 YARPGenericControl::AxisState (int16 axis)
{
	return meid_axis_state(meidx, axis);
}

inline int16 YARPGenericControl::AxisDone (int16 axis)
{
	return meid_axis_done(meidx, axis);
}

inline int16 YARPGenericControl::AxisSource (int16 axis)
{
	return meid_axis_source(meidx, axis);
}

inline int16 YARPGenericControl::DspVelocity (int16 axis,
		double velocity, double duration)
{
	return meid_dsp_velocity(meidx, axis,
		velocity, duration);
}

inline int16 YARPGenericControl::MeiLink (int16 master, int16 slave,
		double ration, int16 source)
{
	return meid_mei_link(meidx, master, slave,
		ration, source);
}

inline int16 YARPGenericControl::EndLink (int16 slave)
{
	return meid_endlink(meidx, slave);
}

inline int16 YARPGenericControl::SetVelocity (int16 axis, double vel)
{
	return meid_set_velocity(meidx, axis, vel);
}

/*
inline int16 YARPGenericControl::CalibrateAxis (int16 axis)
{
	return meid_calibrate_axis(meidx, axis);
}

inline int16 YARPGenericControl::CalibrateMulti (int16 length, int16 *axes)
{
	return meid_calibrate_multi(meidx, length, axes);
}

inline int16 YARPGenericControl::GetAxisRange (int16 axis, int *min, int *max)
{
	return meid_get_axis_range(meidx, axis, min, max);
}

inline int16 YARPGenericControl::CalibrateDifferential (int axis1, int axis2)
{
	return meid_calibrate_differential(meidx, axis1, axis2);
}
*/

inline int16 YARPGenericControl::ZeroAxis (int16 axis)
{
	return meid_set_position (meidx, axis, 0.0);
}

inline int16 YARPGenericControl::GetBootFilter (int16 axis, int16 *coeffs)
{
	return meid_get_boot_filter(meidx, axis, coeffs);
}

inline int16 YARPGenericControl::SetBootFilter (int16 axis, int16 *coeffs)
{
	return meid_set_boot_filter(meidx, axis, coeffs);
}

inline int16 YARPGenericControl::Checksum(void)
{
	return meid_mei_checksum(meidx); // returns the checksum, not an error code
}

/*
inline int16 YARPGenericControl::UploadFirmware (char *fname)
{
	return meid_upload_firmware(meidx, fname);
}

inline int16 YARPGenericControl::DownloadFirmware (char *fname)
{
	return meid_download_firmware(meidx, fname);
}
*/

inline int16 YARPGenericControl::GetAnalog (int16 axis, int16 *value)
{
	return meid_get_analog(meidx, axis, value);
}

inline int16 YARPGenericControl::GetAllAnalog (int *values)
{
	return meid_get_all_analog(meidx, values);
}

inline int16 YARPGenericControl::SetDacOutput (int16 axis, int16 power)
{
	return meid_set_dac_output(meidx, axis, power);
}

/*
inline int16 YARPGenericControl::SendFrame (int axis, double duration, int frame_type,
		  double frame_value)
{
	return meid_send_frame(meidx, axis, duration, frame_type,
		  frame_value);
}
*/

inline int16 YARPGenericControl::ControllerIdle (int16 axis)
{
	return meid_controller_idle(meidx, axis);
}

inline int16 YARPGenericControl::SetDualLoop (int16 axis, int16 vel_axis, int16 dual)
{
	return meid_set_dual_loop(meidx, axis, vel_axis, dual);
}

/*
inline int16 YARPGenericControl::SetAnalog (int axis, int device)
{
	return meid_set_analog(meidx, axis, device);
}
*/

inline int16 YARPGenericControl::ReadAxisAnalog (int16 axis, int16 *value)
{
	return meid_read_axis_analog(meidx, axis, value);
}

inline int16 YARPGenericControl::SetFeedback (int16 axis, int16 device)
{
	return meid_set_feedback(meidx, axis, device);
}

/*
inline int YARPGenericControl::ErrorMsg (int code, char *dst)
{
	return meid_error_msg(meidx, code, dst);
}
*/

inline int16 YARPGenericControl::SetAxisAnalog (int16 axis, int16 state)
{
	return meid_set_axis_analog(meidx, axis, state);
}

inline int16 YARPGenericControl::SetAnalogChannel (int16 axis, int16 channel, int16 differential, int16 bipolar)
{
	return meid_set_analog_channel(meidx, axis, channel, differential, bipolar);
}

inline int16 YARPGenericControl::SetCommand (int16 axis, double position)
{
	return meid_set_command(meidx, axis, position);
}

inline int16 YARPGenericControl::SetEStop (int16 axis)
{
	return meid_set_e_stop(meidx, axis);  
}

/*
inline int16 YARPGenericControl::SetEStopRate (int axis, double rate)
{
	return meid_set_e_stop_rate(meidx, axis, rate);
}

inline int16 YARPGenericControl::GetEStopRate (int axis, double * rate)
{
	return meid_get_e_stop_rate(meidx, axis, rate);
}
*/

inline int16 YARPGenericControl::SetStop (int16 axis)
{
	return meid_set_stop(meidx, axis);  
}

/*
inline int16 YARPGenericControl::SetStopRate (int axis, double rate)
{
	return meid_set_stop_rate(meidx, axis, rate);
}

inline int16 YARPGenericControl::GetStopRate (int axis, double * rate)
{
	return meid_get_stop_rate(meidx, axis, rate);
}
*/

inline int16 YARPGenericControl::ClearStatus (int16 axis)
{
	return meid_clear_status(meidx, axis);
}

// THIS IS WRONG. INPLEMENT meid_motion_done rather than axis_done.
inline int16 YARPGenericControl::MotionDone (int16 axis)
{
	return meid_axis_done(meidx, axis);
}

inline int16 YARPGenericControl::VMove (int16 axis, double velocity, double acceleration)
{
	return meid_v_move(meidx, axis, velocity, acceleration);
}

inline int16 YARPGenericControl::FifoSpace(){
     return meid_fifo_space(meidx);
}

//
// YARPHeadControl class.
//
//NOTE: Comment this out
/*
inline void YARPHeadControl::ConvertToReal(double nod, double roll, double *top, double *bot)
{
	*top =   nod + roll;
	*bot = - nod + roll;
}

inline void YARPHeadControl::ConvertToVirtual(double top, double bot, double *nod, double *roll)
{
  *nod  = 0.5 * (top - bot);
  *roll = 0.5 * (top + bot);
}*/
