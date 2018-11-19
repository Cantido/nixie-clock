#include "Arduino.h"
#include "wwvb.h"

WWVB::WWVB() {
  
}

int WWVB::isTimeSet() {
  return isAligned;
}

void WWVB::nextBit(int b) {
  switch(b) {
    case LOW:
    case HIGH:
      timeFrame[bitIndex] = b;
      break;
    case REFERENCE_BIT:
      if(previousBit == REFERENCE_BIT) {
         timeFrame[60] = {0};
         bitIndex = 0;
      } else {
        checkPosition();
      }
      break;
  }
  if(isAligned == HIGH) {
    triggerDecode();
  }
  previousBit = b;
  bitIndex += 1;
  if(bitIndex >= 60) {
    // It should have gotten reset, but maybe we can recover from missing the reference bit
    bitIndex = 0;
  }
}

void WWVB::tick() {
  timerStart = millis();
}

void WWVB::tock() {
  if(timerStart == 0) {
    return;
  }
  int pulseLength = millis() - timerStart;
  int decodedBit = decodePulseLength(pulseLength);

  nextBit(decodedBit);
}

int WWVB::decodePulseLength(int len) {
  if(len >= 50 && len < 350) {
    return LOW;
  } else if(len >= 350 && len < 650) {
    return HIGH;
  } else if(len >= 650 && len < 950) {
    return REFERENCE_BIT;
  } else {
    return -1;
  }
}

void WWVB::checkPosition() {
  switch(bitIndex) {
    case 0:
      // We are only "aligned" if we have been aligned for the entire minute
      isAligned = HIGH;
      break;
    case 9:
    case 19:
    case 29:
    case 39:
    case 49:
    case 59:
      // If we became aligned during the minute, we aren't totally aligned yet.
      break;
    default:
      isAligned = LOW;
  }
}

void WWVB::triggerDecode() {
  switch(bitIndex) {
    case 9:
      decodeMinutes();
      break;
    case 19:
      decodeHours();
      break;
    case 34:
      decodeDayOfYear();
      break;
    case 54:
      decodeYear();
      break;
    case 56:
      decodeLeapYearIndicator();
      break;
    case 57:
      decodeLeapSecondWarning();
      break;
    case 59:
      decodeDst();
      break;
  }
}

int WWVB::decodeMinutes() {
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

int WWVB::getHour() {
  return decodeHours();
}

int WWVB::getMinute() {
  return decodeMinutes();
}

int WWVB::getSecond() {
  return bitIndex;
}

int WWVB::decodeHours() {
  int hours =
    timeFrame[12] * 20 +
    timeFrame[13] * 10 +
    timeFrame[15] * 8 +
    timeFrame[16] * 4 +
    timeFrame[17] * 2 +
    timeFrame[18] * 1;

  return hours;
}

int WWVB::decodeDayOfYear() {
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

int WWVB::decodeYear() {
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

int WWVB::decodeLeapYearIndicator() {
  return timeFrame[55];
}

int WWVB::decodeLeapSecondWarning() {
  return timeFrame[56];
}

int WWVB::decodeDst() {
  if(timeFrame[57] == LOW && timeFrame[58] == LOW) {
    return ST_ACTIVE;
  } else if (timeFrame[57] == HIGH && timeFrame[58] == HIGH) {
    return DST_ACTIVE;
  } else if (timeFrame[57] == HIGH && timeFrame[58] == LOW) {
    return ST_TO_DST;
  } else if (timeFrame[57] == LOW && timeFrame[58] == HIGH) {
    return DST_TO_ST;
  }
}
