#ifndef A08_CLOCK_H
#define A08_CLOCK_H

#include "a01_config.h"

extern u8 gClockHour;
extern u8 gClockMinute;
extern u8 gClockSecond;
extern bit gClockBlink;

void Clock_Init(void);
u8 Clock_Tick500ms(void);

#endif
