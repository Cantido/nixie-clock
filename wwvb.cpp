#include "Arduino.h"
#include "wwvb.h"

WWVB::WWVB() {
  
}

int WWVB::isTimeSet() {
  return isAligned;
}

void WWVB::tick(int b) {
  switch(b) {
    case LOW:
    case HIGH:
      timeFrame[bitIndex] = b;
      break;
    case REFERENCE_BIT:
      if(previousBit == REFERENCE_BIT) {
        Serial.println("Two reference bits in a row, setting index to 0");
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

void WWVB::checkPosition() {
  switch(bitIndex) {
    case 0:
      Serial.println("Starting 60-second time frame");
      break;
    case 9:
    case 19:
    case 29:
    case 39:
    case 49:
    case 59:
      Serial.println("Position marker, we are correctly-positioned");
      isAligned = HIGH;
      break;
    default:
      Serial.print("Position marker, we are not aligned. Current bit index is ");
      Serial.println(bitIndex);
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
  if(timeFrame[55] == HIGH) {
    Serial.println("Leap year: YES");
  } else {
    Serial.println("Leap year: NO");
  }
  return timeFrame[55];
}

int WWVB::decodeLeapSecondWarning() {
  if(timeFrame[56] == HIGH) {
    Serial.println("Leap second warning: YES");
  } else {
    Serial.println("Leap second warning: NO");
  }
  return timeFrame[56];
}

void WWVB::decodeDst() {
  if(timeFrame[57] == LOW && timeFrame[58] == LOW) {
    Serial.println("Standard time is in effect.");
  } else if (timeFrame[57] == HIGH && timeFrame[58] == HIGH) {
    Serial.println("Daylight savings time is in effect.");
  } else if (timeFrame[57] == HIGH && timeFrame[58] == LOW) {
    Serial.println("The time is changing from ST to DST today");
  } else if (timeFrame[57] == LOW && timeFrame[58] == HIGH) {
    Serial.println("The time is changing from DST to ST today");
  }
}
