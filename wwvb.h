#ifndef wwvb_h
#define wwvb_h
#include "Arduino.h"

#define REFERENCE_BIT 3

#define ST_ACTIVE 0
#define DST_ACTIVE 1
#define ST_TO_DST 2
#define DST_TO_ST 3

#define EPOCH_SECONDS_PER_DAY 86400

class WWVB {
  public:
    WWVB();
    void nextBit(int b);
    void tick();
    void tock();
  private:
    // not zero-indexed
    const static int firstDayOfMonth[13];
    const static int leapYearMonths[13];
    int timerStart = 0; 
    int timeFrame[60];
    int sec = 0;
    int previousBit;
    int isAligned = LOW;
    void checkpoint();
    int getYear();
    int getMonth();
    int getDayOfYear();
    int getDayOfMonth();
    int getHour();
    int getMinute();
    int getSecond();
    int isLeapYear();
    int leapSecondThisMonth();
    int getDstIndicator();
    int decodePulseLength(int len);
};

#endif
