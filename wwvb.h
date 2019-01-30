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
    int getYear(int timeFrame[60]);
    int getMonth(int timeFrame[60]);
    int getDayOfYear(int timeFrame[60]);
    int getDayOfMonth(int timeFrame[60]);
    int getHour(int timeFrame[60]);
    int getMinute(int timeFrame[60]);
    int getSecond();
    tmElements_t getTimeElements(int timeFrame[60]);
    time_t getTime(int timeFrame[60]);
    typedef void(*setExternalTime)(time_t t);
    void setSyncListener(setExternalTime syncListener);
  private:
    // not zero-indexed
    const static int firstDayOfMonth[13];
    const static int leapYearMonths[13];
    const static int bcdWeights[8];
    int atobcd(int arr[], int msbIndex, int lsbIndex);
    int timerStart = 0; 
    int timeFrame[60];
    int sec = 0;
    int previousBit;
    int isAligned = LOW;
    setExternalTime syncListener;
    void checkpoint();
    int isLeapYear(int timeFrame[60]);
    int leapSecondThisMonth(int timeFrame[60]);
    int getDstIndicator(int timeFrame[60]);
    int decodePulseLength(int len);
};

#endif
