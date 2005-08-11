
#ifndef EDHEAD_INC
#define EDHEAD_INC

class EdHead {
private:
  void *system;
  void check();
public:
  EdHead();
  virtual ~EdHead();

  void init();
  
  void set(int x, int y);

  void get(int& x, int& y);
};

#endif

