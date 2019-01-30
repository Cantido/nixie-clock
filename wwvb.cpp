#include <TimeLib.h>
#include "Arduino.h"
#include "wwvb.h"

const uint16_t WWVB::firstDayOfMonth[13] = {
  0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};
const uint16_t WWVB::leapYearMonths[13] = {
  0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335
};

WWVB::WWVB() {
  reset();
}

void WWVB::reset() {
  timerStart = 0;
  timeFrame[60] = {0};
  sec = 0;
  isAligned = LOW;
}

void WWVB::tick() {
  timerStart = millis();
}

void WWVB::tock() {
  if (timerStart == 0) {
    return;
  }
  int pulseLength = millis() - timerStart;
  int decodedBit = decodePulseLength(pulseLength);

  nextBit(decodedBit);
}

uint8_t WWVB::decodePulseLength(int len) {
  if (len >= 50 && len < 350) {
    return LOW;
  } else if (len >= 350 && len < 650) {
    return HIGH;
  } else if (len >= 650 && len < 950) {
    return REFERENCE_BIT;
  } else {
    return -1;
  }
}

void WWVB::nextBit(uint8_t b) {
  if (b != LOW && b != HIGH && b != REFERENCE_BIT) {
    Serial.print("Got an invalid next WWVB bit value: ");
    Serial.println(b);
    reset();
    return;
  }
  
  if (b == REFERENCE_BIT && previousBit == REFERENCE_BIT) {
    Serial.println("starting a new frame");
    reset();
    isAligned = HIGH;
  } else {
    sec += 1;
  } 

  if (b == REFERENCE_BIT) {
    checkpoint();
  }

  timeFrame[sec] = b;

  if(sec == 59 && isAligned) {
    Serial.println("We did it! we got the time locked-down!");
    syncListener(getTime(timeFrame));
  }

  previousBit = b;
}


void WWVB::checkpoint() {
  switch (sec) {
    case 0:
    case 9:
    case 19:
    case 29:
    case 39:
    case 49:
    case 59:
        Serial.print("still aligned at second ");
        Serial.println(sec);
      break;
    default:
      if(isAligned == HIGH) {
        Serial.print("broke alignment, got a reference frame but we are at sec ");
        Serial.println(sec);
      } else {
        Serial.print("still not aligned, got a reference frame but we are at sec ");
        Serial.println(sec);
      }
      reset();
  }
}

void WWVB::setSyncListener(setExternalTime listener) {
  syncListener = listener;
}

tmElements_t WWVB::getTimeElements(uint8_t timeFrame[60]) {
  tmElements_t tm;
  tm.Second = getSecond();
  tm.Minute = getMinute(timeFrame);
  tm.Hour = getHour(timeFrame);
  tm.Day = getDayOfMonth(timeFrame);
  tm.Month = getMonth(timeFrame);
  tm.Year = CalendarYrToTm(getYear(timeFrame));

  return tm;
}

time_t WWVB::getTime(uint8_t timeFrame[60]) {
  return makeTime(getTimeElements(timeFrame));
}

uint8_t WWVB::getSecond() {
  return sec;
}

uint8_t WWVB::getMinute(uint8_t timeFrame[60]) {
  uint8_t minutes =
    timeFrame[1] * 40 +
    timeFrame[2] * 20 +
    timeFrame[3] * 10 +
    timeFrame[5] * 8 +
    timeFrame[6] * 4 +
    timeFrame[7] * 2 +
    timeFrame[8] * 1;

  return minutes;
}

uint8_t WWVB::getHour(uint8_t timeFrame[60]) {
  uint8_t hours =
    timeFrame[12] * 20 +
    timeFrame[13] * 10 +
    timeFrame[15] * 8 +
    timeFrame[16] * 4 +
    timeFrame[17] * 2 +
    timeFrame[18] * 1;

  return hours;
}

uint16_t WWVB::getDayOfYear(uint8_t timeFrame[60]) {
  uint16_t day =
    timeFrame[22] * 200 +
    timeFrame[23] * 100 +
    timeFrame[25] * 80 +
    timeFrame[26] * 40 +
    timeFrame[27] * 20 +
    timeFrame[28] * 10 +
    timeFrame[30] * 8 +
    timeFrame[31] * 4 +
    timeFrame[32] * 2 +
    timeFrame[33] * 1;

  return day;
}

uint16_t WWVB::getYear(uint8_t timeFrame[60]) {
  uint16_t year =
    timeFrame[45] * 80 +
    timeFrame[46] * 40 +
    timeFrame[47] * 20 +
    timeFrame[48] * 10 +
    timeFrame[50] * 8 +
    timeFrame[51] * 4 +
    timeFrame[52] * 2 +
    timeFrame[53] * 1;

  return 2000 + year;
}

uint8_t WWVB::getMonth(uint8_t timeFrame[60]) {
  uint16_t dayOfYear = getDayOfYear(timeFrame);

  if(isLeapYear(timeFrame) == HIGH) {
      for(uint8_t i = 1; i < 13; i++) {
        if(dayOfYear < leapYearMonths[i]) {
          return i-1;
        }
      }
  } else {
      for(uint8_t i = 1; i < 13; i++) {
        if(dayOfYear < firstDayOfMonth[i]) {
          return i-1;
        }
      }
  }

  return 12;
}

uint8_t WWVB::getDayOfMonth(uint8_t timeFrame[60]) {
  uint8_t m = getMonth(timeFrame);
  uint16_t daysBeforeMonth;

  if(isLeapYear(timeFrame) == HIGH) {
    daysBeforeMonth = leapYearMonths[m];
  } else {
    daysBeforeMonth = firstDayOfMonth[m];
  }
  return getDayOfYear(timeFrame) - daysBeforeMonth;
}

bool WWVB::isLeapYear(uint8_t timeFrame[60]) {
  return timeFrame[55] == HIGH;
}

bool WWVB::leapSecondThisMonth(uint8_t timeFrame[60]) {
  return timeFrame[56] == HIGH;
}

daylightSavings_t WWVB::getDstIndicator(uint8_t timeFrame[60]) {
  if (timeFrame[57] == LOW && timeFrame[58] == LOW) {
    return standardTimeInEffect;
  } else if (timeFrame[57] == HIGH && timeFrame[58] == HIGH) {
    return daylightSavingsInEffect;
  } else if (timeFrame[57] == HIGH && timeFrame[58] == LOW) {
    return changeToSavingsToday;
  } else if (timeFrame[57] == LOW && timeFrame[58] == HIGH) {
    return changeToStandardToday;
  }
}
