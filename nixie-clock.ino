#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <Timezone.h> // https://github.com/JChristensen/Timezone
#include <Wire.h>
#include <DS1307RTC.h> // https://www.pjrc.com/teensy/td_libs_DS1307RTC.html
#include "wwvb.h"
#include "tzdata.h"
#include "TimePrint.h"
#include "TimeDisplay.h"
#include "pins.h"

#define NIXIES_ENABLED LOW

#define TZ_INDEX 1

Timezone tz = tzdata[TZ_INDEX];

int previousLevel = LOW;

WWVB wwvb = WWVB();

void setup() {
    pinMode(PDN_PIN, OUTPUT);
    analogReference(INTERNAL);

    digitalWrite(PDN_PIN, HIGH);
    delay(1000);
    digitalWrite(PDN_PIN, LOW);

    wwvb.setSyncListener(doTimeSet);
    setSyncProvider(RTC.get); 
    
    Serial.begin(115200);
}

void doTimeSet(time_t t) {
  // We need to call setTime since it only updates the clock on an interval, and we want it NOW!
  RTC.set(t);
  setTime(t);
}

int timerStart = 0;
int previousBit = -0;

void loop() {
  int sig = analogRead(PWM_IN);
  int pwmSig = (sig > 350);

  if(previousLevel == LOW && pwmSig == HIGH) {
    wwvb.tick();
    TimePrint.print(tz.toLocal(now()));
    if(NIXIES_ENABLED == HIGH) {
      TimeDisplay.show(tz.toLocal(now()));
    }
  } else if(previousLevel == HIGH && pwmSig == LOW) {
    wwvb.tock();
  }

  previousLevel = pwmSig;
  
  delay(50);
}
