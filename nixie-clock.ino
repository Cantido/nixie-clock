#include "wwvb.h"

#define STATUS_BLINK_MS 100

#define PWM_IN 0
#define PDN_PIN 7
#define GREEN_LED 5
#define BLUE_LED 4

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

int timerStart = 0;
int previousBit = -0;

void printTime() {
  int hour = wwvb.getHour();
  int minute = wwvb.getMinute();
  int second = wwvb.getSecond();
  
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
  
  if(second < 10) {
    Serial.print("0");
  }
  Serial.print(second);
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
  Serial.print("ms)");
  printTime();
  return decodedBit;
}

void handleRising() {
  timerStart = millis();
}


void handleBit(int b) {
  wwvb.nextBit(b);
}

void loop() {
  int sig = analogRead(PWM_IN);
  int pwmSig = (sig > 350);

  if(previousLevel == LOW && pwmSig == HIGH) {
    handleRising();
  } else if(previousLevel == HIGH && pwmSig == LOW && timerStart != 0) {
    int decodedBit = handleFalling();
    handleBit(decodedBit);
    timerStart = 0;
  }

  previousLevel = pwmSig;
  
  delay(50);
}
