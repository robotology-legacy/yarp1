#include <stdio.h>
#include <iostream.h>

#include "YARPBottle.h"
#include "YARPPort.h"
#include "YARPTime.h"


YARPOutputPortOf<YARPBottle> out_bot;

void main()
{
  out_bot.Register("/telje_test/o:bot");
  out_bot.Connect("/log/i:bot");
  printf("TELJE TEST is in operation\n");

  int ct = 0;
  while (1)
    {
      printf("Lah di dah\n");
      YARPBottle bot;
      YARPBottleIterator it(bot);
      it.Reset();
      it.WriteVocab(YBC_LOG_PERCEPT);
      it.WriteVocab(YBC_PERCEPT_TEST3);
      it.WriteFloat(3.2);
      it.WriteFloat(4.5+ct);
      SendBottle(bot);
      YARPTime::DelayInSeconds(2);
      ct++;
    }

  /*
  out_bot.Register("/telje_test/o:bot");
  out_bot.Connect("/log/i:bot");
  printf("TELJE TEST is in operation\n");
  while (1)
    {
      //YARPBottleIterator it(out_bot.Content());
      //it.Reset();
      //it.WriteInt(1);
      //it.WriteInt(2);
      //it.WriteInt(3);
      //it.Display();
      out_bot.Content() = 32;
      out_bot.Write();
      YARPTime::DelayInSeconds(2);
    }
   */
}

