#ifndef TimeDisplay_h
#define TimeDisplay_h

#include <TimeLib.h>
#include <Arduino.h>
#include "pins.h"

class TimeDisplay {
  public:
    TimeDisplay();
    static void show(time_t t);
  private:
    static void displayDigit(int displayIndex, int n);
    static void selectDisplay(int n);
    static void writeDigit(int n);
    static int ones(int n) ;
    static int tens(int n) ;
};

extern TimeDisplay TimeDisplay;

#endif
