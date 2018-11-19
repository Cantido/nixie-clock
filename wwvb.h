#ifndef wwvb_h
#define wwvb_h

#include "Arduino.h"

#define REFERENCE_BIT 3

#define ST_ACTIVE 0
#define DST_ACTIVE 1
#define ST_TO_DST 2
#define DST_TO_ST 3

class WWVB {
  public:
    WWVB();
    void nextBit(int b);
    void tick();
    void tock();
    int isTimeSet();
    int getHour();
    int getMinute();
    int getSecond();
  private:
    int timerStart = 0; 
    int timeFrame[60];
    int bitIndex = 0;
    int isAligned = LOW;
    int previousBit;
    void checkPosition();
    void triggerDecode();
    int decodeMinutes();
    int decodeHours();
    int decodeDayOfYear();
    int decodeYear();
    int decodeLeapYearIndicator();
    int decodeLeapSecondWarning();
    int decodeDst();
    int decodePulseLength(int len);
};

#endif
