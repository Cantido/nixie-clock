#ifndef TimePrint_h
#define TimePrint_h

#include <TimeLib.h>
#include <TimeZone.h>

class TimePrint {
  public:
    TimePrint();
    static void print(time_t t, Timezone tz);
  private:
    static void printDigits(int digits);
};

extern TimePrint TimePrint;

#endif
