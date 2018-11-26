#include "TimePrint.h"

void TimePrint::print(time_t t) {
  Serial.print(hourFormat12(t));
  printDigits(minute(t));
  printDigits(second(t));
  Serial.print(" ");
  if(isAM(t)) {
    Serial.print("AM");
  } else {
    Serial.print("PM");
  }
  Serial.print(" ");
  Serial.print(day(t));
  Serial.print(" ");
  Serial.print(monthStr(month(t)));
  Serial.print(" ");
  Serial.print(year(t)); 

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
