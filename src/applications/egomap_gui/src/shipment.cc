#include "YARPPort.h"
#include "YARPBottle.h"

#include "common.h"

YARPOutputPortOf<YARPBottle> out_bot;

void SayBottle(char ch, double arg1, double arg2)
{
  static int registered = 0;
  
  if (!registered)
    {
      out_bot.Register("/egomap_gui/o:bot");
      out_bot.Connect("/egomap/i:bot");
      registered = 1;
    }

  int go = 0;
  
  YARPBottleIterator bot(out_bot.Content());
  bot.Reset();
  bot.WriteVocab(YBC_EGOMAP);
  switch (ch)
    {
    case 't':
      bot.WriteVocab(YBC_TARGET_SET);
      bot.WriteInt(arg1+0.5);
      go = 1;
      break;
    case 'r':
      bot.WriteVocab(YBC_TARGET_REVERT);
      bot.WriteInt(arg1+0.5);
      go = 1;
      break;
    case 'a':
      bot.WriteVocab(YBC_TARGET_APPEAR);
      bot.WriteInt(arg1+0.5);
      go = 1;
      break;
    case 'z':
      bot.WriteVocab(YBC_TARGET_RESET);
      bot.WriteInt(arg1+0.5);
      go = 1;
      break;
    case 'd':
      bot.WriteVocab(YBC_TARGET_ORIENT);
      bot.WriteFloat(arg1);
      bot.WriteFloat(arg2);
      go = 1;
      break;
    case 'i':
      printf("Registering egomap_gui...\n");
      break;
    }

  if (go)
    {
      out_bot.Write();
    }
}
