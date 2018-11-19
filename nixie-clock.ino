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
