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
    void nextBit(const byte b);
    void tick();
    void tock();
    word getYear(const byte timeFrame[60]);
    byte getMonth(const byte timeFrame[60]);
    word getDayOfYear(const byte timeFrame[60]);
    byte getDayOfMonth(const byte timeFrame[60]);
    byte getHour(const byte timeFrame[60]);
    byte getMinute(const byte timeFrame[60]);
    byte getSecond();
    tmElements_t getTimeElements(const byte timeFrame[60]);
    time_t getTime(const byte timeFrame[60]);
    typedef void(*setExternalTime)(time_t t);
    void setSyncListener(const setExternalTime syncListener);
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
    bool isLeapYear(const byte timeFrame[60]);
    bool leapSecondThisMonth(const byte timeFrame[60]);
    daylightSavings_t getDstIndicator(const byte timeFrame[60]);
    byte decodePulseLength(const unsigned long len);
};

#endif
