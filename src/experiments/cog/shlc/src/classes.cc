//
// classes.cc
//

#include "classes.h"

#include <sys/sched.h>

// LATER: include? shlc.h
extern int processing (void);
extern int addcanonical (void);
extern int buildactionmap (int r);
extern int queryactionmap (float theta, float phi, float& torso, int& action_no);

extern int find_object (int& bestaction);
extern int store_last_object_data (void);

//
//
// class PlotVector
void 
PlotVectors::Int_Draw (PtWidget_t *widget, PhRect_t& rect)  
{
	const int Border = 10;

	int height = rect.lr.y - rect.ul.y - 2 * Border;
	int width = rect.lr.x - rect.ul.x - 2 * Border;

	// Do our drawing...
	
	// x spacing
	double x_spacing = double(width) / double(m_frames);

	// look for min, max.
	double min = m_data[0](1);
	double max = min;

	for (int i = 0; i < m_frames; i++)
	{
		for (int j = m_minj; j <= m_maxj; j++)
		{
			double tmp = m_data[i](j);
			if (tmp > max)
				max = tmp;
			else
			if (tmp < min)
				min = tmp;		
		}
	}

	// norm factor...
	if (max-min != 0.0)
	{
		double y_factor = double(height) / (max-min);

		for (int j = m_minj; j <= m_maxj; j++)
		{
			PgSetStrokeColor (PlotColorArray[j-1]);

			double px = 0;
			double py = height - (m_data[0](j) - min) * y_factor;
			double x = x_spacing;
			double y = 0;

			for (int i = 1; i < m_frames; i++)
			{
				y = height - (m_data[i](j) - min)  * y_factor;
				PgDrawILine (int(px)+Border, int(py)+Border, int(x)+Border, int(y)+Border);

				px = x;
				py = y;

				x += x_spacing;
			}
		}

		double y = height + min * y_factor;
		PgSetStrokeColor (Pg_BLACK);
		PgDrawILine (Border, int(y)+Border, width+Border, int(y)+Border);
		PgDrawILine (Border, Border, Border, height+Border);

		double x = m_cur_frame * x_spacing;
		PgDrawILine (Border + int (x), Border, Border + int(x), height+Border);
	}
	else
	{
		PgSetStrokeColor (Pg_BLACK);
		PgDrawILine (Border, height/2+Border, width+Border, height/2+Border);
		PgDrawILine (Border, Border, Border, height+Border);
	}
}


//
// class MessageReceiver

MessageReceiver::MessageReceiver () 
	: m_message_port (YARPInputPort::NO_BUFFERS),
	  m_answer_port (), 
	  m_sema(1) 
	{
		m_reader = NULL;
	}

MessageReceiver::~MessageReceiver () {}

void MessageReceiver::Body ()
{
	int ret = setprio (0, 12);
	if (ret < 0)
	{
		printf ("MessageReceiver - setprio failed, receiver at default priority\n");
	}

	m_message_port.Register (INPUT_MESSAGE_PORTNAME);
	m_answer_port.Register (ANSWER_MESSAGE_PORTNAME);

	m_state = MS_WaitForStart;
	YARPBottle bot;

	for (;;)
	{
		m_message_port.Read();
		YARPBottleIterator iterator(m_message_port.Content());

		printf ("MessageReceiver: something from middle level control\n"); 
		printf ("State is : %d\n", m_state);
		//
		int x = iterator.ReadVocab();

		switch (x)
		{
		default:
		case YBC_SHLC_NONE:
				printf ("MessageReceiver: Empty message received\n");
			break;

		case YBC_SHLC_START_REACHING:
			{
				// tell the reader to start recording.
				printf ("MessageReceiver: Start reaching received\n");
				Lock ();
				if (m_reader != NULL)
				{
					if (m_state == MS_WaitForStart)
					{
						m_reader->StartRecording();
						m_state = MS_WaitForStop;
					}
					else
						printf ("MessageReceiver: Start reaching dropped\n");
				}
				Unlock ();

				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_LOG_PERCEPT);
				it.WriteVocab(YBC_PERCEPT_REACHING_STARTED);
				SendBottle(bot);
			}
			break;

		case YBC_SHLC_STOP_REACHING:
			{
				// tell the reader to stop recording.
				printf ("MessageReceiver: Stop reaching received\n");
				Lock ();
				if (m_reader != NULL)
				{
					if (m_state == MS_WaitForStop)
					{
						m_reader->StopRecording();
						if (m_reader->IsRecordingEnabled())
							m_state = MS_Processing;
						else
							m_state = MS_WaitForStart;
					}
					else
						printf ("MessageReceiver: Stop reaching dropped\n");
				}
				Unlock ();

				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_LOG_PERCEPT);
				it.WriteVocab(YBC_PERCEPT_REACHING_STOPPED);
				SendBottle(bot);
			}
			break;

		case YBC_SHLC_START_RECORDING:
			{
				printf ("MessageReceiver: Start recording received\n");
				Lock ();

				if (m_reader != NULL)
				{
					if (m_state == MS_WaitForStop)
					{
						m_reader->SetProcessingOption (1);
						m_reader->StartPushing(); 
					}
					else
						printf ("MessageReceiver: Start pushing dropped\n");
				}
				Unlock ();

				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_LOG_PERCEPT);
				it.WriteVocab(YBC_PERCEPT_RECORDING_STARTED);
				SendBottle(bot);
			}
			break;

		case YBC_SHLC_STOP_RECORDING:
			{
				printf ("MessageReceiver: Stop recording received\n");
				Lock ();
				if (m_reader != NULL)
				{
					if (m_state == MS_WaitForStop)
					{
						m_reader->SetProcessingOption (1);
						m_reader->StopPushing();
					}
					else
						printf ("MessageReceiver: Stop pushing dropped\n");
				}
				Unlock ();

				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_LOG_PERCEPT);
				it.WriteVocab(YBC_PERCEPT_RECORDING_STOPPED);
				SendBottle(bot);
			}
			break;

		case YBC_SHLC_START_PUSHING:
			{
				printf ("MessageReceiver: Start pushing received\n");
				Lock ();

				int id = iterator.ReadInt();
				printf ("MessageReceiver: action # %d\n", id);

				if (m_reader != NULL)
				{
					m_reader->GetSequenceRef().SetActionID (id);

					if (m_state == MS_WaitForStop)
					{
						m_reader->SetProcessingOption (0);
						m_reader->StartPushing(); 
					}
					else
						printf ("MessageReceiver: Start pushing dropped\n");
				}
				Unlock ();

				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_LOG_PERCEPT);
				it.WriteVocab(YBC_PERCEPT_PUSHING_STARTED);
				SendBottle(bot);
			}
			break;

		case YBC_SHLC_STOP_PUSHING:
			{
				printf ("MessageReceiver: Stop pushing received\n");
				Lock ();
				if (m_reader != NULL)
				{
					if (m_state == MS_WaitForStop)
					{
						m_reader->SetProcessingOption (0);
						m_reader->StopPushing();
					}
					else
						printf ("MessageReceiver: Stop pushing dropped\n");
				}
				Unlock ();

				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_LOG_PERCEPT);
				it.WriteVocab(YBC_PERCEPT_PUSHING_STOPPED);
				SendBottle(bot);
			}
			break;

		case YBC_SHLC_ASK_TORSO:
			{
				printf ("MessageReceiver: Ask torso value received\n");
				
				float theta = iterator.ReadFloat();
				float phi = iterator.ReadFloat();

				// LATER: add here handling of message.
				// given current orientation, return a good torso movement to 
				// maximize the likelihood of good poking.
				float torso = 0;
				int action_no = 0;
				queryactionmap (theta, phi, torso, action_no);
				
				YARPBottleIterator it (m_answer_port.Content());
				it.Reset ();
				it.WriteVocab (YBC_SHLC_REPLY_TORSO);
				it.WriteFloat (torso);
				it.WriteInt (action_no);
				m_answer_port.Write (1);
			}
			break;

		case YBC_SHLC_COMPUTE_BEST_ACTION:
			{
				printf ("MessageReceiver: compute best action message received\n");

				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_LOG_PERCEPT);
				it.WriteVocab(YBC_PERCEPT_REQUEST_ACTION);
				SendBottle(bot);

				if (m_reader != NULL && m_state != MS_Processing)
				{
					int bestaction;
					double before = YARPTime::GetTimeAsSeconds();
					find_object (bestaction);
					double after = YARPTime::GetTimeAsSeconds();

					if (after-before < 0.3)
					{
						YARPBottleIterator it2 (m_answer_port.Content());
						it2.Reset ();
						it2.WriteVocab (YBC_SHLC_COMPUTE_BEST_ACTION_REPLY);
						it2.WriteInt (bestaction);
						m_answer_port.Write ();		// removed Write(1). Deadlock?


						YARPBottleIterator it(bot);
						it.Reset();
						it.WriteVocab(YBC_LOG_PERCEPT);
						it.WriteVocab(YBC_PERCEPT_COMPUTED_ACTION);
						it.WriteFloat (float(bestaction));
						SendBottle(bot);
					}
					else
						printf ("too late, don't bother sending a reply\n");
				}
				else
					printf ("compute best action dropped\n");
			}
			break; 
		}
	}
}


//
// class ImageReader.
//
ImageReader::ImageReader(int w, int h) : 
	  YARPThread(), 
	  m_sema(1), 
	  m_store (FRAMES2STORE, w, h), 
	  m_store_processing (FRAMES2STORE, w, h)
{
	m_array[0].Resize(w,h);
	m_array[0].Zero();

	m_store.Reset();
	m_store_processing.Reset ();

	m_recording_active = false;
	m_recording_enabled = false;

	m_hj.Resize(NumHeadJoints);
	m_hj = 0;
	m_aj.Resize(NumArmJoints);
	m_aj = 0;
	m_ae.Resize(NumArmJoints);
	m_ae = 0;

	m_pushing = false;
	m_delayed_processing = false;

	m_message_receiver = NULL;

	m_processing_option = -1;
}

ImageReader::~ImageReader() {}

void ImageReader::Body()
{
	m_port.Register (INPUT_IMAGE_PORTNAME);
	m_segmented_port.Register (OUTPUT_IMAGE_PORTNAME);
	m_hpos_port.Register (INPUT_HEADP_PORTNAME);
	m_apos_port.Register (INPUT_ARMP_PORTNAME);
	bool reset_pending = false;

	YARPBottle bot;

	for (;;)
	{
		m_port.Read();
		m_hpos_port.Read();
		m_apos_port.Read();

		Lock ();
		YARPGenericImage& tmp = m_port.Content();
		m_array[0].CastCopy(m_port.Content());

		if (m_recording_active)
		{
			//
			for (int i = 1; i <= NumHeadJoints; i++)
				m_hj(i) = m_hpos_port.Content()(i);

			memcpy (m_aj.data(), m_apos_port.Content().j, sizeof(double)*NumArmJoints);

			m_store.AddFrame (m_array[0], m_hj, m_aj, m_ae);

			reset_pending = true;
		}
		else
		if (m_delayed_processing)
		{
			m_recording_enabled = false;

			switch (m_processing_option)
			{
			case -1:
			default:
				{
					printf ("no processing option selected - a bug?\n");
				}
				break;

			case 0:
				{
					YARPBottleIterator it(bot);
					it.Reset();
					it.WriteVocab(YBC_LOG_PERCEPT);
					it.WriteVocab(YBC_PERCEPT_PROCESSING_STARTED);
					SendBottle(bot);

					int ret = processing();

					it.Reset();
					it.WriteVocab(YBC_LOG_PERCEPT);
					it.WriteVocab(YBC_PERCEPT_PROCESSING_FINISHED);
					SendBottle(bot);
					
					// >= 0: ok
					// -1: no contact or other failure
					// -2: no decent flow
					//	
					if (ret >= 0)
					{
						addcanonical ();
					}

					// this uses ret as reinforcement.
					// >= 0 means good
					// < 0 means bad
					buildactionmap (ret);
				}
				break;

			case 1:
				{
					// fiddle w/ the start-stop to process the whole sequence.
					m_store.SetWholeSequenceProcessing();

					YARPBottleIterator it(bot);
					it.Reset();
					it.WriteVocab(YBC_LOG_PERCEPT);
					it.WriteVocab(YBC_PERCEPT_OBSERVING_STARTED);
					SendBottle(bot);

					int ret = processing();

					it.Reset();
					it.WriteVocab(YBC_LOG_PERCEPT);
					it.WriteVocab(YBC_PERCEPT_OBSERVING_FINISHED);
					SendBottle(bot);

					if (ret >= 0)
					{
						// store into objects (YARPObjectContainer) for mimicry.
						store_last_object_data ();
					}
				}
				break;
			}

			m_delayed_processing = false;
			m_recording_enabled = true;
			m_processing_option = -1;

			if (m_message_receiver != NULL)
				m_message_receiver->ProcessingTerminated();
			else
				printf ("WARNING: m_message_receiver is NULL\n");

			reset_pending = false;
		}
		else
		if (reset_pending)
		{
			if (m_message_receiver != NULL)
				m_message_receiver->ProcessingTerminated();
			else
				printf ("WARNING: m_message_receiver is NULL\n");

			reset_pending = false;
		}

		Unlock ();
	}
}

int ImageReader::ProcessWholeSequence (YARPVisualContact& processor)
{
	Lock ();

	m_store_processing.Reset ();
	processor.Reset ();

	for (int i = 0; i < m_store.GetNumElements(); i++)
	{
		// do the processing (add result to m_store_processing).
		// get 
		// m_store.GetImageRef(i), process and return to m_store_processing.GetImageRef(i).

		int contact = processor.Apply (m_store.GetImageRef (i), m_store_processing.GetImageRef(i));
		
		printf ("frame no %d contact : %d\n", i, contact);

		m_store_processing.IncrementCounter ();
	}

	// now is processed.
	m_store_processing.UpToDate() = true;

	Unlock ();

	return 0;
}
