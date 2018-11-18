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

//
//void printTime() {
//  if(hour/2 < 10) {
//    Serial.print("0");
//  }
//  Serial.print(hour/2);
//  Serial.print(":");
//  if(minute < 10) {
//    Serial.print("0");
//  }
//  Serial.print(minute);
//  Serial.print(":");
//  
//  if(bitIndex < 10) {
//    Serial.print("0");
//  }
//  Serial.print(bitIndex);
//  Serial.print(" ");
//  if(hour < 12) {
//    Serial.println("AM");
//  } else {
//    Serial.println("PM");
//  }
//}


int handleFalling() {
  int pulseLength = millis() - timerStart;
  int decodedBit = decodePulseLength(pulseLength);
  Serial.print(decodedBit);
  Serial.print(" (");
  Serial.print(pulseLength);
  Serial.println("ms)");
//  printTime();
  return decodedBit;
}

void handleRising() {
  timerStart = millis();
}


void handleBit(int b) {
  wwvb.tick(b);
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
