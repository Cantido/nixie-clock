#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <Timezone.h> // https://github.com/JChristensen/Timezone
#include "wwvb.h"
#include "tzdata.h"

#define STATUS_BLINK_MS 100

#define PWM_IN 0
#define PDN_PIN 7
#define GREEN_LED 5
#define BLUE_LED 4

#define TZ_INDEX 1

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

    wwvb.setSyncListener(setTime);
    
    Serial.begin(115200);
}

int timerStart = 0;
int previousBit = -0;

void printTime() {
  time_t local = tzdata[TZ_INDEX].toLocal(now());

  Serial.print(hour(local));
  printDigits(minute(local));
  printDigits(second(local));
  Serial.print(" ");
  Serial.print(day(local));
  Serial.print(" ");
  Serial.print(monthStr(month(local)));
  Serial.print(" ");
  Serial.print(year(local)); 

    Serial.print(" ");

  switch(timeStatus()) {
    case timeNotSet:
      Serial.print("(not set yet)");
      break;
    case timeNeedsSync:
      Serial.print("(needs sync)");
      break;
    case timeSet:
      Serial.print("(set and synced)");
      break;
  }
  
  Serial.println("");
}

void printDigits(int digits){
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
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
