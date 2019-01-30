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
    void nextBit(uint8_t b);
    void tick();
    void tock();
    uint16_t getYear(uint8_t timeFrame[60]);
    uint8_t getMonth(uint8_t timeFrame[60]);
    uint16_t getDayOfYear(uint8_t timeFrame[60]);
    uint8_t getDayOfMonth(uint8_t timeFrame[60]);
    uint8_t getHour(uint8_t timeFrame[60]);
    uint8_t getMinute(uint8_t timeFrame[60]);
    uint8_t getSecond();
    tmElements_t getTimeElements(uint8_t timeFrame[60]);
    time_t getTime(uint8_t timeFrame[60]);
    typedef void(*setExternalTime)(time_t t);
    void setSyncListener(setExternalTime syncListener);
    void reset();
  private:
    // not zero-indexed
    const static uint16_t firstDayOfMonth[13];
    const static uint16_t leapYearMonths[13];
    int timerStart = 0;
    uint8_t timeFrame[60];
    uint8_t sec = 0;
    uint8_t previousBit;
    uint8_t isAligned = LOW;
    setExternalTime syncListener;
    void checkpoint();
    bool isLeapYear(uint8_t timeFrame[60]);
    bool leapSecondThisMonth(uint8_t timeFrame[60]);
    daylightSavings_t getDstIndicator(uint8_t timeFrame[60]);
    uint8_t decodePulseLength(int len);
};

#endif
