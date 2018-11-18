#include "wwvb.h"

#define WWVB_LOW_MIN_MS 100
#define WWVB_LOW_MAX_MS 300
#define WWVB_HIGH_MIN_MS 400
#define WWVB_HIGH_MAX_MS 600
#define WWVB_REF_MIN_MS 700
#define WWVB_REF_MAX_MS 800

#define STATUS_BLINK_MS 100

#define PWM_IN 0
#define PDN_PIN 7
#define GREEN_LED 5
#define BLUE_LED 4

#define REFERENCE_BIT 3

#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12

// not zero-indexed
int firstDayOfMonth[13] = {
  -1, 1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335
};
int leapYearMonths[13] = {
  -1, 1, 32, 61, 92, 122, 153, 183, 214, 245, 275, 306, 336
};

int previousLevel = LOW;

WWVB wwvb = WWVB();

void setup() {
    pinMode(PDN_PIN, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    analogReference(INTERNAL);


    digitalWrite(PDN_PIN, HIGH);
    delay(1000);
    digitalWrite(PDN_PIN, LOW);
    
    Serial.begin(115200);

}


int decodePulseLength(int len) {
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

int timeFrame[60] = {-1};
int bitIndex = 0;

int isAligned = LOW;

int timerStart = 0;
int previousBit = -0;

int hour = 0;
int minute = 0;

void printTime() {
  if(hour/2 < 10) {
    Serial.print("0");
  }
  Serial.print(hour/2);
  Serial.print(":");
  if(minute < 10) {
    Serial.print("0");
  }
  Serial.print(minute);
  Serial.print(":");
  
  if(bitIndex < 10) {
    Serial.print("0");
  }
  Serial.print(bitIndex);
  Serial.print(" ");
  if(hour < 12) {
    Serial.println("AM");
  } else {
    Serial.println("PM");
  }
}


int handleFalling() {
  int pulseLength = millis() - timerStart;
  int decodedBit = decodePulseLength(pulseLength);
  Serial.print(decodedBit);
  Serial.print(" (");
  Serial.print(pulseLength);
  Serial.print("ms) -- ");
  printTime();
  return decodedBit;
}

void handleRising() {
  timerStart = millis();
}


void handleBit(int b) {
  switch(b) {
    case LOW:
    case HIGH:
      timeFrame[bitIndex] = b;
      wwvb.tick(b);
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
    minute += 1;
    if(minute >= 60) {
      minute = 0;
      hour += 1;
      if(hour >= 24) {
        hour = 0;
  
      }
    }
  }
}

void decodeMinutes() {
  int minutes =
    timeFrame[1] * 40 +
    timeFrame[2] * 20 +
    timeFrame[3] * 10 +
    timeFrame[5] * 8 +
    timeFrame[6] * 4 +
    timeFrame[7] * 2 +
    timeFrame[8] * 1;
  minute = minutes;
  Serial.print("Decoded frame: ");
  Serial.print(minutes);
  Serial.println(" minutes");
}

void decodeHours() {
  int hours =
    timeFrame[12] * 20 +
    timeFrame[13] * 10 +
    timeFrame[15] * 8 +
    timeFrame[16] * 4 +
    timeFrame[17] * 2 +
    timeFrame[18] * 1;
  hour = hours;
  Serial.print("Decoded frame: ");
  Serial.print(hours);
  Serial.println(" hours");
}

void decodeDayOfYear() {
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
  Serial.print("Decoded frame: ");
  Serial.print(day);
  Serial.println(" day of year");
}

void decodeYear() {
  int year =
    timeFrame[45] * 80 +
    timeFrame[46] * 40 +
    timeFrame[47] * 20 +
    timeFrame[48] * 10 +
    timeFrame[50] * 8 +
    timeFrame[51] * 4 +
    timeFrame[52] * 2 +
    timeFrame[53] * 1;
  Serial.print("Decoded frame: year 20");
  Serial.println(year);
}

void decodeLeapYearIndicator() {
  if(timeFrame[55] == HIGH) {
    Serial.println("Leap year: YES");
  } else {
    Serial.println("Leap year: NO");
  }
}

void decodeLeapSecondWarning() {
  if(timeFrame[56] == HIGH) {
    Serial.println("Leap second warning: YES");
  } else {
    Serial.println("Leap second warning: NO");
  }
}

void decodeDst() {
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
void checkPosition() {
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

void triggerDecode() {
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

void loop() {
  int sig = analogRead(PWM_IN);
  int pwmSig = (sig > 350);
//
//  Serial.print(sig);
//  Serial.print(" ");
//  Serial.print(pwmSig * 512);
//  Serial.print(" ");
  
  if(previousLevel == LOW && pwmSig == HIGH) {
    handleRising();
  } else if(previousLevel == HIGH && pwmSig == LOW && timerStart != 0) {
    int decodedBit = handleFalling();
    handleBit(decodedBit);
    timerStart = 0;
  }

//  Serial.print(previousBit);
//  Serial.println("");

  previousLevel = pwmSig;
  
  delay(50);
}
