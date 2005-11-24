
#include <yarp/YARPImage.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>

#include "Grapher.h"

#include <iostream>
using namespace std;

Grapher graph;


class MainThread : public YARPThread {
public:
  bool stop;

  YARPString name;

  MainThread() {
    stop = false;
  }

  void setName(const char *nname) {
    name = nname;
  }

  void Run(const char *name) {
    YARPInputPortOf<YARPGenericImage> input;
    input.Register(name);
    while (!stop) {
      input.Read();
      YARPImageOf<YarpPixelRGB> rgb;
      rgb.CastCopy(input.Content());
      graph.Apply(rgb);
    }
  }

  virtual void Body() {
    Run(name.c_str());
  }

  void Stop() {
    stop = true;
    YARPTime::DelayInSeconds(1);
  }
};


int main(int argc, char *argv[]) {
  if (argc<2) {
    printf("Please supply a name for an input port e.g. \"/foo\"\n");
  }

  MainThread _thread;
  _thread.setName(argv[1]);
  _thread.Begin();
  
  char c = 0;
  
  do
    {
      cout << "Type q+return to quit" << endl;
      cin >> c;
    }
  while (c != 'q');

  cout << "ending..." << endl;

  _thread.Stop();

  graph.End();

  return 0;
}

