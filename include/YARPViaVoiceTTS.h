#ifndef YARPViaVoiceTTS_INC
#define YARPViaVoiceTTS_INC

enum VoiceQuality
{
  VQ_PITCH,
  VQ_HEADSIZE,
  VQ_ROUGHNESS,
  VQ_BREATHINESS,
  VQ_RANGE,
  VQ_SPEED,
  VQ_VOLUME,
  
  VQ_COUNT // marks end
};

enum VoiceClass
{
  VC_MALE,
  VC_FEMALE,
  VC_CHILD,
  VC_OLD_MALE,
  VC_OLD_FEMALE,
};

class YARPViaVoiceTTS
{
public:

  float vq[VQ_COUNT];
  int vq_set[VQ_COUNT];
  int vc;

  YARPViaVoiceTTS();

  void SetClass(int n_voice_class);
  void SetQuality(int n_voice_quality, float value);

  int Say(const char *text, int rehearsal=0);

  int Sayable(const char *text)
    { return Say(text,1); }
};


#endif
