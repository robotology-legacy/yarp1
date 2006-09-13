#ifndef LISTEN_INC
#define LISTEN_INC

#ifdef __cplusplus
extern "C" {
#endif

  void init_listen();

  void plisten(unsigned char *sample, int len);

#ifdef __cplusplus
};
#endif

#endif
