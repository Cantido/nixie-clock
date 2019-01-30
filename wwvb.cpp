#include <TimeLib.h>
#include "Arduino.h"
#include "wwvb.h"

const int WWVB::firstDayOfMonth[13] = {
  -1, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};
const int WWVB::leapYearMonths[13] = {
  -1, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335
};

const int WWVB::bcdWeights[10] = {
  1, 2, 4, 8, 10, 20, 40, 80, 100, 200
};

WWVB::WWVB() {

}

void WWVB::nextBit(int b) {
  if (b == REFERENCE_BIT && previousBit == REFERENCE_BIT) {
    Serial.println("starting a new frame");
    timeFrame[60] = {0};
    isAligned = HIGH;
    sec = 0;
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

void WWVB::setSyncListener(setExternalTime listener) {
  syncListener = listener;
}

tmElements_t WWVB::getTimeElements(int timeFrame[60]) {
  tmElements_t tm;
  tm.Second = getSecond();
  tm.Minute = getMinute(timeFrame);
  tm.Hour = getHour(timeFrame);
  tm.Day = getDayOfMonth(timeFrame);
  tm.Month = getMonth(timeFrame);
  tm.Year = CalendarYrToTm(getYear(timeFrame));

  return tm;
}

time_t WWVB::getTime(int timeFrame[60]) {
  return makeTime(getTimeElements(timeFrame));
}

int WWVB::decodePulseLength(int len) {
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
      isAligned = LOW;
  }
}

int WWVB::getSecond() {
  return sec;
}

int WWVB::getMinute(int timeFrame[60]) {
  return atobcd(timeFrame, 1, 8);
}

int WWVB::getHour(int timeFrame[60]) {
  return atobcd(timeFrame, 12, 18);
}

int WWVB::getDayOfYear(int timeFrame[60]) {
  return atobcd(timeFrame, 22, 33);
}

int WWVB::getYear(int timeFrame[60]) {
  return 2000 + atobcd(timeFrame, 45, 53);
}


int WWVB::atobcd(int arr[], int msbIndex, int lsbIndex) {
  int result = 0;
  for(int i = msbIndex; i <= lsbIndex; i++) {
    result += timeFrame[i] * bcdWeights[lsbIndex - i];
  }
  return result;
}

int WWVB::getMonth(int timeFrame[60]) {
  int dayOfYear = getDayOfYear(timeFrame);

  if(isLeapYear(timeFrame) == HIGH) {
      for(int i = 1; i < 13; i++) {
        if(dayOfYear < leapYearMonths[i]) {
          return i-1;
        }
      }
  } else {
      for(int i = 1; i < 13; i++) {
        if(dayOfYear < firstDayOfMonth[i]) {
          return i-1;
        }
      }
  }

  return 12;
}

int WWVB::getDayOfMonth(int timeFrame[60]) {
  int m = getMonth(timeFrame); 
  int daysBeforeMonth;
  
  if(isLeapYear(timeFrame) == HIGH) {
    daysBeforeMonth = leapYearMonths[m];
  } else {
    daysBeforeMonth = firstDayOfMonth[m];
  }
  return getDayOfYear(timeFrame) - daysBeforeMonth;
}

int WWVB::isLeapYear(int timeFrame[60]) {
  return timeFrame[55];
}

int WWVB::leapSecondThisMonth(int timeFrame[60]) {
  return timeFrame[56];
}

int WWVB::getDstIndicator(int timeFrame[60]) {
  if (timeFrame[57] == LOW && timeFrame[58] == LOW) {
    return ST_ACTIVE;
  } else if (timeFrame[57] == HIGH && timeFrame[58] == HIGH) {
    return DST_ACTIVE;
  } else if (timeFrame[57] == HIGH && timeFrame[58] == LOW) {
    return ST_TO_DST;
  } else if (timeFrame[57] == LOW && timeFrame[58] == HIGH) {
    return DST_TO_ST;
  }
}
