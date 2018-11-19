#include <TimeLib.h>
#include "Arduino.h"
#include "wwvb.h"

const int WWVB::firstDayOfMonth[13] = {
  -1, 1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335
};
const int WWVB::leapYearMonths[13] = {
  -1, 1, 32, 61, 92, 122, 153, 183, 214, 245, 275, 306, 336
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
    setTime(getHour(), getMinute(), getSecond(), getDayOfMonth(), getMonth(), getYear());
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

int WWVB::getHour() {
  int hours =
    timeFrame[12] * 20 +
    timeFrame[13] * 10 +
    timeFrame[15] * 8 +
    timeFrame[16] * 4 +
    timeFrame[17] * 2 +
    timeFrame[18] * 1;

  return hours;
}

int WWVB::getMinute() {
  int minutes =
    timeFrame[1] * 40 +
    timeFrame[2] * 20 +
    timeFrame[3] * 10 +
    timeFrame[5] * 8 +
    timeFrame[6] * 4 +
    timeFrame[7] * 2 +
    timeFrame[8] * 1;

  return minutes;
}

int WWVB::getSecond() {
  return sec;
}

int WWVB::getMonth() {
  int dayOfYear = getDayOfYear();

  if(isLeapYear() == HIGH) {
      for(int i = 1; i < 13; i++) {
        if(dayOfYear > leapYearMonths[i]) {
          return i;
        }
      }
  } else {
      for(int i = 1; i < 13; i++) {
        if(dayOfYear > firstDayOfMonth[i]) {
          return i;
        }
      }
  }

  return 0;
}

int WWVB::getDayOfYear() {
  int day =
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

int WWVB::getDayOfMonth() {
  int m = getMonth(); 

  if(isLeapYear() == HIGH) {
    return getDayOfYear() - leapYearMonths[m];
  } else {
    return getDayOfYear() - firstDayOfMonth[m];
  }
}

int WWVB::getYear() {
  int year =
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

int WWVB::isLeapYear() {
  return timeFrame[55];
}

int WWVB::leapSecondThisMonth() {
  return timeFrame[56];
}

int WWVB::getDstIndicator() {
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
