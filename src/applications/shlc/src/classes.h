//
// classes.h for the shlc application.
//

#ifndef __classesh__
#define __classesh__

#include "displayimg.h"

#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPTime.h"

#include "YARPVisualContact.h"
#include "YARPVisualSearch.h"
#include "YARPFlowTracker.h"
#include "YARPBottle.h"

#include "portnames.h"

#include "conf/tx_data.h"

const int FRAMES2STORE = 400;

class ImageReader;

//
// to draw in a RawWidget.
class PlotSimple 
{
public:
	PlotSimple () {}
	virtual ~PlotSimple () {}

	virtual void Int_Draw(PtWidget_t *widget, PhRect_t& rect) = 0;

	void Draw (PtWidget_t *widget, PhTile_t *damage)
	{
	    PtSuperClassDraw( PtBasic, widget, damage );
		PhRect_t rect;

		//find our canvas 
		PtBasicWidgetCanvas( widget, &rect );

		//clip to our basic canvas (it's only polite).
		PtClipAdd( widget, &rect );

		PhPoint_t translation;
		translation.x = rect.ul.x;
		translation.y = rect.ul.y;
		PgSetTranslation (&translation, Pg_RELATIVE);

		// Specialized draw.
		Int_Draw (widget, rect);

		translation.x = -rect.ul.x;
		translation.y = -rect.ul.y;
		PgSetTranslation (&translation, Pg_RELATIVE);

		//remove our clipping
		PtClipRemove();
	}
};


const int PlotColorArray[12] = 
{    
	Pg_WHITE,
	Pg_MAGENTA,
	Pg_GREEN,
	Pg_RED,
	Pg_YELLOW,
	Pg_BLUE,
	Pg_WHITE,
	Pg_MAGENTA,
	Pg_GREEN,
	Pg_RED,
	Pg_YELLOW,
	Pg_BLUE
};

//
// class PlotVector
class PlotVectors : public PlotSimple
{
protected:
	CVisDVector *m_data;
	int m_frames;
	int m_cur_frame;
	int m_minj;
	int m_maxj;

public:
	PlotVectors (int frames, int size, int min, int max) : PlotSimple ()
	{
		assert (max-min+1 <= 12);
		assert (min >= 1 && max <= size && max >= min && max-min+1 <= size);

		m_frames = frames;
		m_cur_frame = 0;

		m_minj = min;
		m_maxj = max;
	}
	
	virtual ~PlotVectors() {}

	inline void SetDataPtr (CVisDVector *d) { m_data = d; }
	inline void SetNumElements (int el) { m_frames = el; }
	inline void SetCurFrame (int f) { m_cur_frame = f; }

	virtual void Int_Draw(PtWidget_t *widget, PhRect_t& rect);
};

//
// class MessageReceiver.
//
class MessageReceiver : public YARPThread
{
protected:
	ImageReader *m_reader;
	YARPSemaphore m_sema;
	YARPInputPortOf<YARPBottle> m_message_port;
	YARPOutputPortOf<YARPBottle> m_answer_port;

	enum { MS_WaitForStart = 0, MS_WaitForStop = 1, MS_Processing = 2 };
	int m_state;

	inline void Lock () { m_sema.Wait(); }
	inline void Unlock () { m_sema.Post(); }

public:
	MessageReceiver ();
	virtual ~MessageReceiver ();

	inline void Start (bool w=false) { Begin(); }
	inline void Terminate (bool w=false) { End(); }

	inline void SetReader (ImageReader *r) { m_reader = r; }
	inline void ProcessingTerminated (void) 
	{
		Lock ();
		m_state = MS_WaitForStart;
		Unlock ();
	}
	virtual void Body ();
};


//
// handle image storage and communication.
class ImageReader : public YARPThread
{
protected:
	YARPSemaphore m_sema;
	YARPInputPortOf<YARPGenericImage>	m_port;
	YARPOutputPortOf<YARPGenericImage>  m_segmented_port;
	
	YARPInputPortOf<JointPos>			m_hpos_port;
	YARPInputPortOf<ArmJoints>			m_apos_port;
	// MISSING: port for the arm position error.

	YARPImageOf<YarpPixelBGR> m_array[1];

	YARPImageSequence m_store;
	YARPOutputImageSequence m_store_processing;
	
	bool m_recording_active;

	bool m_recording_enabled;	// enable/disable recording.
	bool m_pushing;

	bool m_delayed_processing;
	MessageReceiver *m_message_receiver;

	CVisDVector m_hj;
	CVisDVector m_aj;
	CVisDVector m_ae;

	inline void Lock () { m_sema.Wait(); }
	inline void Unlock () { m_sema.Post(); }

	int m_processing_option;

public:
	ImageReader(int w, int h);
	virtual ~ImageReader();

	virtual void Body();

	inline void Start (bool w=false) { Begin(); }
	inline void Terminate (bool w=false) { End(); }

	inline void Read (YARPGenericImage& img)
	{
		Lock ();
		img.CastCopy (m_array[0]);
		Unlock ();
	}

	// start/stop actual recording.
	inline void StartRecording () 
	{
		Lock ();
		if (m_recording_enabled)
		{
			m_recording_active = true;
			m_store.Reset ();
			m_store_processing.Reset ();
		}
		Unlock ();
	}
	
	inline void StopRecording() 
	{
		Lock ();
		if (m_recording_enabled && m_recording_active)
		{
			if (m_pushing)
			{
				m_store.SetStopPushing();
				m_pushing = false;
				m_delayed_processing = true;
			}
			m_recording_active = false;
		}
		Unlock ();
	}

	inline bool IsRecording ()
	{
		bool ret = false;
		Lock ();
		ret = m_recording_active;
		Unlock ();
		return ret;
	}

	inline void SetProcessingOption (int o)
	{
		Lock ();
		m_processing_option = o;
		Unlock ();
	}

	inline void StartPushing ()
	{
		Lock ();
		if (m_recording_active)
		{
			m_store.SetStartPushing();
			m_pushing = true;
			m_delayed_processing = false;
		}
		Unlock ();
	}

	inline void StopPushing ()
	{
		Lock ();
		if (m_recording_active)
		{
			m_store.SetStopPushing();
			m_pushing = false;
			m_delayed_processing = true;
		}
		Unlock ();
	}

	// enable/disable recording.
	inline void EnableRecording()
	{
		Lock ();
		m_recording_enabled = true;
		Unlock ();
	}

	inline void DisableRecording()
	{
		Lock ();
		m_recording_enabled = false;
		Unlock ();
	}

	inline void ToggleRecording()
	{
		Lock ();
		m_recording_enabled = !m_recording_enabled;
		Unlock ();
	}

	inline bool IsRecordingEnabled ()
	{
		bool ret = false;
		Lock ();
		ret = m_recording_enabled;
		Unlock ();
		return ret;
	}

	// get the ref for manipulating the sequence.
	// MT access not guaranteed. please make sure to access the store only
	// when the recording is disabled.
	
	inline bool SafeAccess(void)
	{
		// LATER: return true if it's safe to access the memory store.
		return true;
	}

	YARPImageSequence& GetSequenceRef (void) { return m_store; }
	YARPImageSequence* GetSequencePtr (void) { return &m_store; }
	YARPOutputPortOf<YARPGenericImage>& GetSegmentedImagePort(void) { return m_segmented_port; }

	// this is actually single threaded (but not thread-safe anyway).
	YARPOutputImageSequence& GetOutputSequenceRef (void) { return m_store_processing; }

	// process the whole sequence in memory (if any) and stores results in m_store_prcessing.
	int ProcessWholeSequence (YARPVisualContact& processor);

	// return the delayed_processing flag.
	bool GetDelayedProcessingFlag (void)
	{
		bool ret = false;
		Lock ();
		ret = m_delayed_processing;
		Unlock ();
		return ret;
	}

	void SetMessageReceiver (MessageReceiver *thread) { m_message_receiver = thread; }

	int GetCurrentFrame (YARPImageOf<YarpPixelBGR>& img)
	{
		Lock ();
		img.PeerCopy (m_array[0]);
		Unlock ();

		return 0;
	}
};


// handle the live display only.
class InputManager
{
protected:
	YARPOutofScreenImage m_oos;
	YARPImageOf<YarpPixelBGR> m_tmp;
	ImageReader *m_reader;

	PtWidget_t *m_widget;
	
	bool m_frozen; 

public:
	InputManager (int w, int h) : m_oos(w, h)
	{
		m_frozen = false;
		m_widget = NULL;
		m_reader = NULL;

		m_tmp.Resize (w, h);
		m_tmp.Zero();
	}

	~InputManager () {}

	inline void Display()
	{
		if (!m_frozen && m_widget != NULL && m_reader != NULL)
		{
			m_reader->Read (m_tmp);
			m_oos.Display (m_widget, m_tmp);
		}
	}

	inline void SetWidget (PtWidget_t *w) { m_widget = w;	}
	inline void SetChannel (ImageReader *r) { m_reader = r; }

	inline void Freeze() { m_frozen = true; }
	inline void Unfreeze() { m_frozen = false; }
	inline void ToggleFreeze() { m_frozen = !m_frozen; }
};



#endif