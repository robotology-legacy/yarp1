#include <assert.h>

#include "YARPTime.h"
#include "YARPPort.h"
#include "YARPBottle.h"


class YIP : public YARPInputPortOf<YARPBottle>
{
public:
  YIP() : YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS)
    {
    }

  virtual void OnRead()
    {
      assert(Read());
      YARPBottleIterator it(Content());
      printf(">>> "); it.Display();
    }
} in_bot;

//extern int __debug_level;

void main()
{
//  __debug_level = 100;
  in_bot.Register("/view_bot/i:bot");
  YARPPort::Connect("/behave/o:bot","/view_bot/i:bot");
  while (1)
    {
      YARPTime::DelayInSeconds(500);
    }
}
