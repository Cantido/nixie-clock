#ifndef wwvb_h
#define wwvb_h

#include "Arduino.h"

#define REFERENCE_BIT 3

class WWVB {
  public:
    WWVB();
    void tick(int b);
  private:
    int timeFrame[60];
    int bitIndex = 0;
    int isAligned = LOW;
    int previousBit;
    void checkPosition();
    void triggerDecode();
    void decodeMinutes();
    void decodeHours();
    void decodeDayOfYear();
    void decodeYear();
    void decodeLeapYearIndicator();
    void decodeLeapSecondWarning();
    void decodeDst();
};

#endif
