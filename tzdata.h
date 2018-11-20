#ifndef tzdata_h
#define tzdata_h

#include <Timezone.h> // https://github.com/JChristensen/Timezone

TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};
TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};

TimeChangeRule usEST = {"EST", Second, Sun, Nov, 2, -300};
TimeChangeRule usCST = {"CST", Second, Sun, Nov, 2, -360};
TimeChangeRule usMST = {"MST", Second, Sun, Nov, 2, -420};
TimeChangeRule usPST = {"PST", Second, Sun, Nov, 2, -480};

Timezone tzdata[] = {
  Timezone(usEDT, usEST),
  Timezone(usCDT, usCST),
  Timezone(usMDT, usMST),
  Timezone(usPDT, usPST)
};

#endif
