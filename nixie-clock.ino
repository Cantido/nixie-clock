#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
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

int timerStart = 0;
int previousBit = -0;

void printTime() {

  int h = hour();
  int m = minute();
  int s = second();
  
  if(h/2 < 10) {
    Serial.print("0");
  }
  Serial.print(h/2);
  Serial.print(":");
  if(m < 10) {
    Serial.print("0");
  }
  Serial.print(m);
  Serial.print(":");
  
  if(s < 10) {
    Serial.print("0");
  }
  Serial.print(s);
  Serial.print(" ");
  if(isAM()) {
    Serial.print("AM");
  } else {
    Serial.print("PM");
  }
//  Serial.print(" -- ");
//  Serial.print(wwvb.getTimestamp());
  
  Serial.print(" -- ");

  switch(timeStatus()) {
    case timeNotSet:
      Serial.print("not set yet");
      break;
    case timeNeedsSync:
      Serial.print("needs sync");
      break;
    case timeSet:
      Serial.print("set and synced");
      break;
  }
  
  Serial.println("");
}

void loop() {
  int sig = analogRead(PWM_IN);
  int pwmSig = (sig > 350);

  if(previousLevel == LOW && pwmSig == HIGH) {
    wwvb.tick();
  } else if(previousLevel == HIGH && pwmSig == LOW) {
    wwvb.tock();
    printTime();
  }

  previousLevel = pwmSig;
  
  delay(50);
}
