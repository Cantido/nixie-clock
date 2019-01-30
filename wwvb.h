#ifndef wwvb_h
#define wwvb_h
#include "Arduino.h"

#define REFERENCE_BIT 3

typedef enum {
  standardTimeInEffect, daylightSavingsInEffect, changeToStandardToday, changeToSavingsToday
} daylightSavings_t;

class WWVB {
  public:
    WWVB();
    void nextBit(byte b);
    void tick();
    void tock();
    word getYear(byte timeFrame[60]);
    byte getMonth(byte timeFrame[60]);
    word getDayOfYear(byte timeFrame[60]);
    byte getDayOfMonth(byte timeFrame[60]);
    byte getHour(byte timeFrame[60]);
    byte getMinute(byte timeFrame[60]);
    byte getSecond();
    tmElements_t getTimeElements(byte timeFrame[60]);
    time_t getTime(byte timeFrame[60]);
    typedef void(*setExternalTime)(time_t t);
    void setSyncListener(setExternalTime syncListener);
    void reset();
  private:
    // not zero-indexed
    const static word firstDayOfMonth[13];
    const static word leapYearMonths[13];
    unsigned long timerStart = 0;
    byte timeFrame[60];
    byte sec = 0;
    byte previousBit;
    byte isAligned = LOW;
    setExternalTime syncListener;
    void checkpoint();
    bool isLeapYear(byte timeFrame[60]);
    bool leapSecondThisMonth(byte timeFrame[60]);
    daylightSavings_t getDstIndicator(byte timeFrame[60]);
    byte decodePulseLength(unsigned long len);
};

#endif
