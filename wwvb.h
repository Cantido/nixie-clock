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
    int getYear();
    int getMonth();
    int getDayOfYear();
    int getDayOfMonth();
    int getHour();
    int getMinute();
    int getSecond();
    tmElements_t getTimeElements();
    time_t getTime();
    typedef void(*setExternalTime)(time_t t);
    void setSyncListener(setExternalTime syncListener);
  private:
    // not zero-indexed
    const static int firstDayOfMonth[13];
    const static int leapYearMonths[13];
    int timerStart = 0; 
    int timeFrame[60];
    int sec = 0;
    int previousBit;
    int isAligned = LOW;
    setExternalTime syncListener;
    void checkpoint();
    int isLeapYear();
    int leapSecondThisMonth();
    int getDstIndicator();
    int decodePulseLength(int len);
};

#endif
