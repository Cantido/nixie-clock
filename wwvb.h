#ifndef wwvb_h
#define wwvb_h

#include "Arduino.h"

#define REFERENCE_BIT 3

class WWVB {
  public:
    WWVB();
    void tick(int b);
  private:
    int _timeFrame[60];
};

#endif
