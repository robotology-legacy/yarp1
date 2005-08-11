
#ifndef EDHEADREMOTE_INC
#define EDHEADREMOTE_INC

class EdHeadRemote {
private:
  void *system;
  void check();
public:
  EdHeadRemote();
  virtual ~EdHeadRemote();

  void init();
  
  void set(int x, int y);

  void get(int& x, int& y);
};

#endif

