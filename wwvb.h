#ifndef wwvb_h
#define wwvb_h

#include "Arduino.h"

#define REFERENCE_BIT 3

class WWVB {
  public:
    WWVB();
    void nextBit(int b);
    
    int isTimeSet();
    int getHour();
    int getMinute();
    int getSecond();
  private:
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
    void decodeDst();
};

#endif
