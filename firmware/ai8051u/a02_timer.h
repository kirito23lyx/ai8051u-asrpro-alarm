#ifndef A02_TIMER_H
#define A02_TIMER_H

#include "a01_config.h"

extern bit f1ms;
extern bit f500ms;
extern bit fBeepEn;

void Timer0_Init(void);
void Timer1_Init(void);
void Timer1_SetReload(u16 reload);

#endif
