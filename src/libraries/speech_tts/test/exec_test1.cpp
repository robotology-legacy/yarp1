
#include "YARPViaVoiceTTS.h"

void main()
{
  YARPViaVoiceTTS master;
  master.SetQuality(VQ_VOLUME,0.75);
  master.SetQuality(VQ_PITCH,0.75);
  master.Say("boo");
}
