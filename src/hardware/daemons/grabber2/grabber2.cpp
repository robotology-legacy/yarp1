#define UNIX_BUILD

//#include "debug-new.h"

#include "YARPAll.h"

#include <iostream.h>
#include <iomanip.h>
#include <map.h>
#include <algo.h>
#include <stdio.h>
#include <sys/kernel.h>
#include <time.h>
#include <signal.h>

//#include "Port.h"
#include "mesh.h"
//#include "Image.h"
#include "Framegrabber.h"

//#include "Sendables.h"
//#include "ImageSendable.h"

//#include "PortOf.h"

#include "YARPPort.h"
#include "YARPImagePortContent.h"
#include "YARPNetworkTypes.h"
#include "YARPImage.h"
#include "YARPThread.h"


//#define SUPPORT_SOK

//#include "delay-patch.h"

//#include "debug-new.h"

extern int __debug_level;

#define DEBUG

//#define TEST_DELAY 1000

//#define HAS_GRABBER

class ImageProcessor : public BasedLink<ImageProcessor>
{
public:
  int update_id;
  
  ImageProcessor() : update_id(0) {}

  YARPGenericImage img;
  virtual void Lock()   {}
  virtual void Unlock() {}
  virtual void Update() {}

  void Update(int id);
  //GenericImage& GetImage();
};



void ImageProcessor::Update(int id)
{
  if (id != update_id)
    {
      if (GetSource()!=NULL)
	{
	  GetSource()->Update(id);
	}
      Update();
      update_id = id;
    }
}

class ImageProcessorMesh : public MeshOf<ImageProcessor>
{
public:
  int last_update_id;

  ImageProcessorMesh() : last_update_id(0) {}
  void Update();
};


void ImageProcessorMesh::Update()
{
  int update_id = !last_update_id;
  ImageProcessor *cursor = GetRoot();
  
  while (cursor!=NULL)
    {
      cursor->Update(update_id);
      cursor = cursor->GetMeshNext();
    }

  last_update_id = update_id;
}

// prototypes.
void FindProcessor (const char *name, int chain);
void AddProcessor (ImageProcessor *p, int chain);


#ifdef SUPPORT_SOK
// Can't allocate grabber statically here, must do it in the
// sok child process
#include "SokPuppet.h"
PXCFramegrabber *p_grabber = NULL;
template <class T>
T *SafeP(T *p)
{
  assert (p!=NULL);
  return p;
}
#define grabber (*(SafeP(p_grabber)))

#else
PXCFramegrabber grabber;
#endif

int switch_rgb_bgr = 0;

class ImageGrabber : public ImageProcessor
{
public:
  Frame f;
  int acquired;
  int sending_ok;
  int channel;		// 1 or 2.

  ImageGrabber(int ch) 
    {
      //grabber.Init();
      acquired = 0;
	  assert (ch == 1 || ch == 2);
	  channel = ch;
    }

  virtual void Update()
    {
      if (acquired)
	{
	  grabber.RelinquishFrame(f, channel);
	  acquired = 0;
	}
      grabber.AcquireFrame(f,1, channel);
      acquired = 1;
      if (switch_rgb_bgr)
	{
	  char *b, *r, *last;
	  char tmp;
	  b = f.GetRawBuffer();
	  r = f.GetRawBuffer()+2;
	  last = f.GetRawBuffer()+f.GetHeight()*f.GetWidth()*f.GetPixelSize();
	  if (f.GetPixelSize()==3)
	    {
	      while (r<last)
		{
		  tmp = *r;
		  *r = *b;
		  *b = tmp;
		  b += 3;
		  r += 3;
		}
	    }
	  else if (f.GetPixelSize()==4)
	    {
	      while (r<last)
		{
		  tmp = *r;
		  *r = *b;
		  *b = tmp;
		  b += 4;
		  r += 4;
		}
	    }
	  switch (f.GetID())
	    {
	    case YARP_PIXEL_RGB:
	      f.CastID(YARP_PIXEL_BGR);
	      break;
	    case YARP_PIXEL_BGR:
	      f.CastID(YARP_PIXEL_RGB);
	      break;
	      /*
	    case IMAGE_TYPE_RGBA:
	      f.CastID(IMAGE_TYPE_BGRA);
	      break;
	    case IMAGE_TYPE_BGRA:
	      f.CastID(IMAGE_TYPE_RGBA);
	      break;
	      */
	    }
	}
      img.Refer(f);
    }
};

class ImageTestPattern : public ImageProcessor
{
public:
  int offset;
  ImageTestPattern() 
    {
      img.Resize(128,128);
      offset = 0;
    }
  virtual void Update()
    {
      for (int i=0; i<128*128; i++)
	{
	  img.GetRawBuffer()[i] = 10*((i+offset)%10);
	}
      //delay(100);
      offset++;
    }
};

class ImageResizer : public ImageProcessor
{
public:
  int h, w;
  ImageResizer(int n_h, int n_w) :
    h(n_h), w(n_w) {}

  virtual void Update()
    {
      if (GetSource()!=NULL)
	{
	  //cout << "Starting copy" << endl;
	  //	  img.Copy(GetSource()->img);
	  img.Refer(GetSource()->img);
	  //cout << "Done copy" << endl;
	}
    }  
};


class ImageTyper : public ImageProcessor
{
public:
  int format;
  ImageTyper(int n_format)
    { format = n_format; }

   virtual void Update()
    {
      if (GetSource()!=NULL)
	{
	  //cout << "Starting copy for format " << format << endl;
	  img.SetID(format);
	  img.CastCopy(GetSource()->img);
	  //cout << "Done copy" << endl;
	}
    }  
 
};

class ImagePorter : public ImageProcessor
{
public:
  String port_name;
  YARPBasicOutputPort<YARPImagePortContent> port;

  ImagePorter(const char *n_port_name, int legacy) : port_name(n_port_name)
    {
      port.Register(n_port_name);
      if (legacy)
	{
	  //	  port.SetLegacyMode(1);
	}
    }

  virtual void Update()
    {
      if (GetSource()!=NULL)
	{
	  img.Refer(GetSource()->img);

	  //	  port.Sendable().AddHeaders(0,1);
	  port.Content().Refer(img);
	  port.Write();
	}
    }
};

#ifdef SUPPORT_SOK
#include "ImageSokker.h"
int sokked = 0;
#endif

// Returns time in seconds, with fractional accuracy to the limits of
// the resolution of the real time clock
double GetTime()
{
  struct timespec tspec;
  clock_gettime(CLOCK_REALTIME,&tspec);
  return tspec.tv_nsec/1000000000.0 + tspec.tv_sec;
}

class ImageTimer : public ImageProcessor
{
public:
  int count;
  int reset_count;
  double full_count;
  double last, first;
  double average10;

  ImageTimer()
    {
      count = 0;
      full_count = 0;
      reset_count = 0;
      average10 = 0;
      first = last = GetTime();
    }

  virtual void Update()
    {
      if (GetSource()!=NULL)
	{
	  img.Refer(GetSource()->img);
	}
      count++;
      full_count += 1;
      double now = GetTime();
      if (now-last>1)
	{
	  double average = full_count/(now-first);
	  reset_count++;
	  cout << "About " << count << " frames per second (average over 10 seconds: "
	       << setiosflags(ios::fixed)
	       << setprecision(2) << average10 << ")" << endl;
	  
	  last = now;
	  count = 0;
	  if (reset_count>=10)
	    {
	      average10 = average;
	      full_count = 0;
	      first = now;
	      reset_count = 0;
	    }
	}
    }
};





struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

struct ltint
{
  bool operator()(int s1, int s2) const
  {
    return s1 < s2;
  }
};


template<class T> struct print : public unary_function<T, void>
{
  print(ostream& out) : os(out), count(0) {}
  void operator() (T& x) { os << x.first << "=" << x.second.name << ' '; ++count; }
  ostream& os;
  int count;
};

enum
{
  PARAM_HEIGHT,
  PARAM_WIDTH,
  PARAM_PIXEL,
  PARAM_LEFT,
  PARAM_TOP,
  PARAM_RIGHT,
  PARAM_BOTTOM,
  PARAM_TOP_OFFSET,
  PARAM_LEFT_OFFSET,
  PARAM_OUTPUT,
  PARAM_REFORMAT,
  PARAM_RESCALE,
  PARAM_INPUT,
  PARAM_NAME,
  PARAM_PATTERN,
  PARAM_TIMER,
  PARAM_SOK,
  PARAM_DEBUG,
  PARAM_SUBIMAGE,
  PARAM_SQUARE,
  PARAM_LEGACY,
  PARAM_TAGGED,
};

const char *TXT_SQUARE_LEFT = "0.125";
const char *TXT_SQUARE_RIGHT = "0.875";
const char *TXT_SQUARE_TOP = "0";
const char *TXT_SQUARE_BOTTOM = "1";

class ParameterItem
{
public:
  int index;
  int count;
  const char *name;
  int type;
  ParameterItem()
    {}

  ParameterItem(int n_index, int n_count, const char *n_name, int n_type) :
    index(n_index), count(n_count), name(n_name), type(n_type)
    {}
};

typedef map<const char*, ParameterItem, ltstr> ParameterMap;
typedef map<const char*, const char *, ltstr> ShortcutMap;
typedef map<const char*, int, ltstr> IntMap;

ParameterMap parameter;
int grabber_started = 0;

ImageProcessorMesh mesh;
ImageProcessor *current_processor1 = NULL;
ImageProcessor *current_processor2 = NULL;
int mesh_started = 0;

void InitMesh()
{
  ImageProcessor *p;

  mesh.SetAutoClear(1);

  p = new ImageGrabber(1);
  assert(p!=NULL);
  p->SetOwner(&mesh);
  p->SetLabel("root1");
  current_processor1 = p;

  p = new ImageGrabber(2);
  assert(p!=NULL);
  p->SetOwner(&mesh);
  p->SetLabel("root2");
  current_processor2 = p;
}

void AddTo(ImageProcessor *p, const char *name, int chain)
{
	assert (p != NULL);
	FindProcessor(name, chain);
	AddProcessor(p, chain);
}

void AddProcessor(ImageProcessor *p, int chain)
{
  assert(p!=NULL);
  p->SetOwner(&mesh);

  if (chain == 1)
  {
	 p->SetSource(current_processor1);
	 current_processor1 = p;
  }
  else
  {
	 p->SetSource(current_processor2);
	 current_processor2 = p;
  }
}

void FindProcessor(const char *name, int chain)
{
  char *tmp = new char[strlen(name) + 10];
  assert (tmp != NULL);

  sprintf (tmp, "%s%d\0", name, chain);

  ImageProcessor *p = mesh.GetByLabel(tmp);
  if (p!=NULL)
    {
	  if (chain == 1)
		current_processor1 = p;
	  else
		current_processor2 = p;
    }
  else
    {
      cerr << "Failed to locate processor " << tmp << endl;
      exit(1);
    }

  delete[] tmp;
}

void LabelProcessor(const char *name, int chain)
{
  char *tmp = new char[strlen(name) + 10];
  assert (tmp != NULL);

  if (chain == 1)
  {
	sprintf (tmp, "%s1\0", name);
	current_processor1->SetLabel(tmp);
  }
  else
  {
	sprintf (tmp, "%s2\0", name);
	current_processor2->SetLabel(tmp);
  }

  delete[] tmp;
}

/*
void Test()
{
  // what is the sequence of operations that the arguments to the grabber
  // can initiate?
  ImageProcessor *p, *prev;

  p = new ImageGrabber();
  p->SetOwner(&mesh);
  p->SetLabel("root");
  prev = p;

  p = new ImageResizer(50,60);
  p->SetOwner(&mesh);
  p->SetSource(prev);
  prev = p;

  p = new ImagePorter("/cam/trial/color");
  p->SetOwner(&mesh);
  p->SetSource(prev);

  prev = mesh.GetByLabel("root");
  if (prev!=NULL)
    {
      p = new ImageResizer(30,30);
      p->SetOwner(&mesh);
      p->SetSource(prev);
      prev = p;
      
      p = new ImagePorter("/cam/trial/mono");
      p->SetOwner(&mesh);
      p->SetSource(prev);
    }

  // Sanity test
  mesh.Update();
  mesh.Update();
  mesh.Update();
  mesh.Update();
  mesh.Update();
  mesh.Update();
  mesh.Update();
  mesh.Update();
}
*/

IntMap value;

void DemandArgs(const char *flag_name, int actual, int want, int more_allowed = 1)
{
  if (want>actual)
    {
      cerr << "Too few arguments suppled for " << flag_name << endl;
      exit(1);
    }
  if (want<actual && !more_allowed)
    {
      cerr << "Too many arguments suppled for " << flag_name << endl;
      exit(1);
    }
}

void ProcessFlag(const char *flag_name, int argc, char *argv[])
{
  static int legacy = 1;

  ParameterItem& item = parameter[flag_name];
  //  cout << "Flag " << flag_name << " (default=" << item.name << ") with " 
  //       << argc << " arguments" << endl;
  //  cout << "argc " << argc << " argv " << argv[0] << endl;
  switch (item.index)
    {
    case PARAM_OUTPUT:
		{
		  DemandArgs(flag_name,argc,1,0);
		  cout << "Starting native output " << argv[0];
		  cout << (legacy?" (legacy format)":" (tagged format)");
		  cout << endl;

		  char *buffer = new char[strlen(argv[0])+10];
		  sprintf (buffer, "%s.1\0", argv[0]);
		  AddProcessor(new ImagePorter(buffer,legacy), 1);
		  sprintf (buffer, "%s.2\0", argv[0]);
		  AddProcessor(new ImagePorter(buffer,legacy), 2);
		  delete[] buffer;
		}
      break;
    case PARAM_TAGGED:
      legacy = 0;
      break;
    case PARAM_LEGACY:
      legacy = 1;
      break;
    case PARAM_RESCALE:
      {
	int p1, p2;
	DemandArgs(flag_name,argc,2,0);
	p1 = atoi(argv[0]);
	p2 = atoi(argv[1]);
	cout << "Adding scaling object (" << p1 << "," << p2 << ")" << endl;
	AddProcessor(new ImageResizer(p1,p2), 1);
	AddProcessor(new ImageResizer(p1,p2), 2);
      }
      break;
    case PARAM_REFORMAT:
      {
	int p1;
	DemandArgs(flag_name,argc,1,0);
	p1 = atoi(argv[0]);
	if (p1==0 && value.find(argv[0])!=value.end())
	  {
	    p1 = value[argv[0]];
	  }
	cout << "Adding reformat object (" << argv[0] << ")" << endl;
	AddProcessor(new ImageTyper(p1),1);
	AddProcessor(new ImageTyper(p1),2);
      }
      break;
    case PARAM_NAME:
      DemandArgs(flag_name,argc,1,0);
      LabelProcessor(argv[0], 1);
      LabelProcessor(argv[0], 2);
      break;
    case PARAM_INPUT:
      DemandArgs(flag_name,argc,1,0);
      FindProcessor(argv[0], 1);
      FindProcessor(argv[0], 2);
      break;
    case PARAM_PATTERN:
      cout << "Adding test pattern object" << endl;
      AddProcessor(new ImageTestPattern(),1);
      AddProcessor(new ImageTestPattern(),2);
      break;
    case PARAM_TIMER:
      cout << "Adding timer object" << endl;
      AddProcessor(new ImageTimer(),1);
      AddProcessor(new ImageTimer(),2);
      break;
    case PARAM_SOK:
#ifdef SUPPORT_SOK
      DemandArgs(flag_name,argc,1,0);
      cout << "Starting Sok output " << argv[0] << endl;
      //AddProcessor(new ImagePorter(argv[0]));
      sokked = 1;
#else
      cout << "Sok not supported at this time" << endl;
      exit(1);
#endif
      break;
    case PARAM_DEBUG:
      DemandArgs(flag_name,argc,1,0);
      __debug_level = atoi(argv[0]);
      cout << "Setting debug level to " 
	   << __debug_level << endl;
      break;
    case PARAM_SUBIMAGE:
      {
	double p1, p2, p3, p4;
	DemandArgs(flag_name,argc,4,0);
	p1 = atof(argv[0]);
	p2 = atof(argv[1]);
	p3 = atof(argv[2]);
	p4 = atof(argv[3]);
	cout << "Setting subimage (top=" << p1 << ", right=" << p2 
	     << ", bottom=" << p3 << ", right=" << p4
	     << ")" << endl;
	parameter["top"].name = argv[0];
	parameter["left"].name = argv[1];
	parameter["bottom"].name = argv[2];
	parameter["right"].name = argv[3];
      }
      break;
    case PARAM_SQUARE:
      {
	cout << "Setting aspect ratio for square pixels" << endl;
	cout << "(currently only does the right thing if the output image size is square)" 
	     << endl;
	parameter["top"].name = TXT_SQUARE_TOP;
	parameter["left"].name = TXT_SQUARE_LEFT;
	parameter["bottom"].name = TXT_SQUARE_BOTTOM;
	parameter["right"].name = TXT_SQUARE_RIGHT;
      }
      break;
    default:
      DemandArgs(flag_name,argc,1,0);
      cout << "Setting " << flag_name << " to " << argv[0] << endl;
      parameter[flag_name].name = argv[0];
      break;
    }
}


class UpdateThread : public YARPThread
{
public:
  virtual void Body();
};

void UpdateThread::Body()
{
  delay(500);
  while (1)
    {
      mesh.Update();
#ifdef TEST_DELAY
      delay(TEST_DELAY);
#endif
      Yield();
    }
}

#ifdef SUPPORT_SOK
SokPuppet puppet;
#endif

static void terminate(int sig)
{
  YARPThread::PrepareForDeath();
  grabber.EmergencyStop();
  exit(1);
}

int main(int argc, char *argv[])
{

  char buf[256];
  ShortcutMap shortcut;
  int have_flag = 0;
  //  ParameterItem *flag_item = NULL;
  const char *flag_name;
  char **flag_argv;
  int flag_argc = 0;

  InitMesh();

  shortcut["h"] = "height";
  shortcut["w"] = "width";
  shortcut["p"] = "pixel";
  shortcut["i"] = "input";
  shortcut["o"] = "output";
  shortcut["format"] = "pixel format";
  shortcut["f"] = "pixel format";
  shortcut["pixel"] = "pixel format";
  shortcut["v"] = "debug";
  shortcut["verbose"] = "debug";
  shortcut["d"] = "debug";
  shortcut["sub"] = "subimage";
  shortcut["tag"] = "tagged";

  parameter["height"] = ParameterItem(PARAM_HEIGHT,1,"128",0);
  parameter["width"] = ParameterItem(PARAM_WIDTH,1,"128",0);
  parameter["left"] = ParameterItem(PARAM_LEFT,1,"0",0);
  parameter["right"] = ParameterItem(PARAM_RIGHT,1,"1",0);
  parameter["top"] = ParameterItem(PARAM_RIGHT,1,"0",0);
  parameter["bottom"] = ParameterItem(PARAM_RIGHT,1,"1",0);
  parameter["dleft"] = ParameterItem(PARAM_LEFT_OFFSET,1,"0",0);
  parameter["dtop"] = ParameterItem(PARAM_TOP_OFFSET,1,"0",0);
  parameter["pixel format"] = ParameterItem(PARAM_PIXEL,1,"mono",0);
  parameter["subimage"] = ParameterItem(PARAM_SUBIMAGE,4,"*",1);
  parameter["output"] = ParameterItem(PARAM_OUTPUT,1,"*",1);
  parameter["rescale"] = ParameterItem(PARAM_RESCALE,2,"*",1);
  parameter["reformat"] = ParameterItem(PARAM_REFORMAT,1,"*",1);
  parameter["input"] = ParameterItem(PARAM_INPUT,1,"*",1);
  parameter["name"] = ParameterItem(PARAM_NAME,1,"*",1);
  parameter["pattern"] = ParameterItem(PARAM_PATTERN,1,"*",1);
  parameter["timer"] = ParameterItem(PARAM_TIMER,1,"*",1);
  parameter["sok"] = ParameterItem(PARAM_SOK,1,"*",1);
  parameter["square"] = ParameterItem(PARAM_SQUARE,0,"*",1);
  parameter["debug"] = ParameterItem(PARAM_DEBUG,1,"*",1);
  parameter["tagged"] = ParameterItem(PARAM_TAGGED,0,"*",1);
  parameter["legacy"] = ParameterItem(PARAM_LEGACY,0,"*",1);

  value["mono"] = Framegrabber::PIXEL_MONO;
  value["rgb"] = Framegrabber::PIXEL_RGB;
  //value["rgba"] = Framegrabber::PIXEL_RGBA;
  value["bgr"] = Framegrabber::PIXEL_BGR;


  argc--;
  argv++;

  while (argc>0)
    {
      const char *cursor = argv[0];
      if (*cursor == '-')
	{
	  if (have_flag)
	    {
	      ProcessFlag(flag_name,flag_argc, flag_argv);
	      have_flag = 0;
	    }

	  const char *name;
	  cursor++;
	  if (*cursor == '-')
	    {
	      // allow double hyphen syntax
	      cursor++;
	    }
	  name = cursor;
	  if (parameter.find(name) == parameter.end())
	    {
	      if (shortcut.find(name) != shortcut.end())
		{
		  name = shortcut[name];
		}
	      else
		{
		  name = NULL;
		}
	    }
	  if (name!=NULL)
	    {
	      have_flag = 1;
	      flag_name = name;
	      flag_argc = 0;
	      flag_argv = NULL;
	    }
	  else
	    {
	      cerr << "Unrecognized parameter: \"" << cursor << "\"" << endl;
	      exit(1);
	    }
	}
      else
	{
	  if (flag_argc == 0)
	    {
	      flag_argv = argv;
	      //cout << "Setting argv to " << flag_argv[0] << endl;
	    }
	  flag_argc++;
	}
      argc--;
      argv++;  
    }

  if (have_flag)
    {
      ProcessFlag(flag_name,flag_argc, flag_argv);
      have_flag = 0;
    }

#ifdef DEBUG
  //  for_each(parameter.begin(),parameter.end(),
  //     print<pair<const char* const,ParameterItem> >(cout));
  //  cout << endl;
#endif

  int h, w ,p;
  double left, right, top, bottom, dleft, dtop;
  h = atoi(parameter["height"].name);
  w = atoi(parameter["width"].name);
  left = atof(parameter["left"].name);
  right = atof(parameter["right"].name);
  top = atof(parameter["top"].name);
  bottom = atof(parameter["bottom"].name);
  dleft = atof(parameter["dleft"].name);
  dtop = atof(parameter["dtop"].name);
  if (value.find(parameter["pixel format"].name)!=value.end())
    {
      p = value[parameter["pixel format"].name];
      if (p==Framegrabber::PIXEL_RGB) // || p==Framegrabber::PIXEL_RGBA)
	{
	  cout << "*** Color byte order is [b g r] from this framegrabber." << endl;
	  cout << "*** Adding conversion step to [r g b] order." << endl;
	  switch_rgb_bgr = 1;
	  if (p==Framegrabber::PIXEL_RGB) p = Framegrabber::PIXEL_BGR;
	  //if (p==Framegrabber::PIXEL_RGBA) p = Framegrabber::PIXEL_BGRA;
	}
    }
  else
    {
      cout << "Unrecognized pixel format " << parameter["pixel format"].name << endl;
      cout << "Supported formats are {mono, rgb, rgba, bgr, bgra}" << endl;
      exit(1);
    }
  assert (h>0 && w>0);
  

#ifdef SUPPORT_SOK
  if (sokked)
    {
      puppet.SetName("grabber");
      puppet.Begin(1);
    }
  p_grabber = new PXCFramegrabber;
  assert(p_grabber!=NULL);

#endif

  grabber.SetSize(h,w);
  grabber.SetSubImage(top,left,bottom,right,dleft,dtop);
  grabber.SetFormat((Framegrabber::PixelFormat)p);
  grabber.Init();

  UpdateThread updates;
  updates.Begin();

  // Set up signal handlers for framegrabber process.
  signal(SIGTERM, &terminate);
  signal(SIGINT, &terminate);
  
  grabber.Main();
  

  return 0;
}

