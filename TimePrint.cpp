#include "TimePrint.h"

void TimePrint::print(time_t t, Timezone tz) {
  time_t local = tz.toLocal(t);

  Serial.print(hourFormat12(local));
  printDigits(minute(local));
  printDigits(second(local));
  Serial.print(" ");
  if(isAM(local)) {
    Serial.print("AM");
  } else {
    Serial.print("PM");
  }
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

void TimePrint::printDigits(int digits){
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
