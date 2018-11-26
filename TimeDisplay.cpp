#include "TimeDisplay.h"

void TimeDisplay::show(time_t t) {
  displayDigit(SECONDS_ONES_DISPLAY, ones(second(t)));
  displayDigit(SECONDS_TENS_DISPLAY, tens(second(t)));
  
  displayDigit(MINUTES_ONES_DISPLAY, ones(minute(t)));
  displayDigit(MINUTES_TENS_DISPLAY, tens(minute(t)));
  
  displayDigit(HOURS_ONES_DISPLAY, ones(hour(t)));
  displayDigit(HOURS_TENS_DISPLAY, tens(hour(t)));
}

void TimeDisplay::displayDigit(int displayIndex, int n) {
  selectDisplay(displayIndex);
  writeDigit(n);
}

void TimeDisplay::writeDigit(int n) {
  digitalWrite(DIGIT_BIT_0_PIN, n & 0b0000001);
  digitalWrite(DIGIT_BIT_1_PIN, n & 0b0000010);
  digitalWrite(DIGIT_BIT_2_PIN, n & 0b0000100);
  digitalWrite(DIGIT_BIT_3_PIN, n & 0b0001000);
}

void TimeDisplay::selectDisplay(int n) {
  digitalWrite(SECONDS_ONES_DISPLAY, n == SECONDS_ONES_DISPLAY);
  digitalWrite(SECONDS_TENS_DISPLAY, n == SECONDS_TENS_DISPLAY);
  
  digitalWrite(MINUTES_ONES_DISPLAY, n == MINUTES_ONES_DISPLAY);
  digitalWrite(MINUTES_TENS_DISPLAY, n == MINUTES_TENS_DISPLAY);
  
  digitalWrite(HOURS_ONES_DISPLAY, n == HOURS_ONES_DISPLAY);
  digitalWrite(HOURS_TENS_DISPLAY, n == HOURS_TENS_DISPLAY);
}

int TimeDisplay::ones(int n) {
  return n % 10;
}

int TimeDisplay::tens(int n) {
  return ones(n / 10);
}
